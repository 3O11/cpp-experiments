#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#define IMGUI_IMPL_METAL_CPP
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_metal.h>
#include <imgui/imgui.h>

#include <Math/Vector.hpp>
#include <Math/Matrix.hpp>
#include <Math/Utils/Transform.hpp>
#include <Math/Quaternion.hpp>
#include <Cube.hpp>
#include <stb_image.h>

#include "metalUtil.h"

#include <simd/simd.h>

struct Camera
{
    Math::Matrix4f view;
    Math::Matrix4f projection;
};

struct Instance
{
    Math::Matrix4f transform;
};

int main(int argc, char **argv)
{
    int32_t prevWindowWidth = 640;
    int32_t prevWindowHeight = 480;
    int32_t windowWidth = 640;
    int32_t windowHeight = 480;

    float movementSpeed = 0.05f;
    Math::Vector3f position;
    Math::Vector3f yawPitchRoll;
    float sensitivity = 0.05f;
    Math::Vector2d prevMousePos;
    Math::Vector2d mousePos;

    std::cout << "Hello world!\n";
    std::cout << sizeof(Utils::Vertex) << "\n";
    std::cout << sizeof(simd::float3) << "\n";
    std::cout << sizeof(simd::float2) << "\n";

    // Init GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    // Create window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // Init basic Metal stuff
    MTL::Device *metalDevice = MTL::CreateSystemDefaultDevice();
    MTL::CommandQueue *metalCommandQueue = metalDevice->newCommandQueue();

    // Prepare the layer for drawing
    float windowWidthScale, windowHeightScale;
    glfwGetWindowContentScale(window, &windowWidthScale, &windowHeightScale);
    CA::MetalLayer *metalLayer = CA::MetalLayer::layer()->retain();
    metalLayer->setDevice(metalDevice);
    metalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    metalLayer->setDrawableSize({windowWidth * windowWidthScale, windowHeight * windowHeightScale});
    setLayer(glfwGetCocoaWindow(window), metalLayer);

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGuiContext *imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable
                               |  ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplMetal_Init(metalDevice);
    bool imguiShowDemo = true;

    // Load a texture into memory
    int32_t imageWidth, imageHeight, imageChannels;
    uint8_t *imageData = stbi_load("117.jpeg", &imageWidth, &imageHeight, &imageChannels, 4);
    MTL::TextureDescriptor *metalTextureDescriptor =
        MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormatRGBA8Unorm,
                                                    imageWidth,
                                                    imageHeight,
                                                    false);
    metalTextureDescriptor->setUsage(MTL::TextureUsageShaderRead);
    MTL::Texture *metalTexture = metalDevice->newTexture(metalTextureDescriptor);
    metalTexture->replaceRegion({0, 0, 0, (uint32_t)imageWidth, (uint32_t)imageHeight, 1}, 0, imageData, imageWidth * 4);
    stbi_image_free(imageData);

    // Allocate (and fill some of) GPU memory buffers
    auto cube = Utils::Align(Utils::MakeCube());
    MTL::Buffer *cubeVertices
        = metalDevice->newBuffer(cube.Vertices.size() * sizeof(Utils::Vertex), MTL::ResourceStorageModeManaged);
    MTL::Buffer *cubeIndices
        = metalDevice->newBuffer(cube.Indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeManaged);

    memcpy(cubeVertices->contents(), &(cube.Vertices[0]), cube.Vertices.size() * sizeof(Utils::Vertex));
    memcpy(cubeIndices->contents(),  &(cube.Indices[0]),  cube.Indices.size()  * sizeof(uint16_t));

    cubeVertices->didModifyRange(NS::Range::Make(0, cubeVertices->length()));
    cubeIndices->didModifyRange(NS::Range::Make(0, cubeIndices->length()));

    MTL::Buffer *cubeInstances
        = metalDevice->newBuffer(sizeof(Math::Matrix4f), MTL::ResourceStorageModeManaged);
    MTL::Buffer *cameraData
        = metalDevice->newBuffer(2 * sizeof(Math::Matrix4f), MTL::ResourceStorageModeManaged);

    // Prepare the shader
    std::ifstream shaderFile("shader.metal");
    std::stringstream shaderReadBuffer;
    shaderReadBuffer << shaderFile.rdbuf();
    std::string shaderSource = shaderReadBuffer.str();
    shaderReadBuffer.clear();
    shaderFile.close();

    using NS::StringEncoding::UTF8StringEncoding; // This alias is temporary
    NS::Error *shaderError;
    MTL::Library *shaderLibrary =
        metalDevice->newLibrary(NS::String::string(shaderSource.c_str(), UTF8StringEncoding), nullptr, &shaderError);
    if (!shaderLibrary)
    {
        std::cout << shaderError->localizedDescription()->utf8String() << std::endl;
        // TODO: Properly dispose of existing state
        return -1;
    }

    MTL::Function *vertexFunc = shaderLibrary->newFunction(NS::String::string("vertexMain", UTF8StringEncoding));
    MTL::Function *fragmentFunc = shaderLibrary->newFunction(NS::String::string("fragmentMain", UTF8StringEncoding));
    
    MTL::RenderPipelineDescriptor *pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    pipelineDescriptor->setVertexFunction(vertexFunc);
    pipelineDescriptor->setFragmentFunction(fragmentFunc);
    pipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);
    pipelineDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormat::PixelFormatDepth16Unorm);

    MTL::RenderPipelineState *pipelineState =
        metalDevice->newRenderPipelineState(pipelineDescriptor, &shaderError);
    if (!pipelineState)
    {
        std::cout << shaderError->localizedDescription()->utf8String() << std::endl;
        // TODO: Properly dispose of existing state
        return -1;
    }

    vertexFunc->release();
    fragmentFunc->release();
    pipelineDescriptor->release();
    
    // Prepare the depth buffer
    MTL::TextureDescriptor *depthTextureDescriptor
        = MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormatDepth16Unorm,
                                                      windowWidth * windowWidthScale,
                                                      windowHeight * windowHeightScale,
                                                      false);
    depthTextureDescriptor->setStorageMode(MTL::StorageModePrivate);
    depthTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
    MTL::Texture *depthBuffer = metalDevice->newTexture(depthTextureDescriptor);

    MTL::DepthStencilDescriptor *depthDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
    depthDescriptor->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLess);
    depthDescriptor->setDepthWriteEnabled(true);
    MTL::DepthStencilState *depthStencilState = metalDevice->newDepthStencilState(depthDescriptor);
    depthDescriptor->release();

    bool done = false;
    while(!done)
    {
        // Update the window and layer size
        glfwPollEvents();
        done = glfwWindowShouldClose(window);
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        if (windowWidth != prevWindowWidth || windowHeight != prevWindowHeight)
        {
            glfwGetWindowContentScale(window, &windowWidthScale, &windowHeightScale);
            metalLayer->setDrawableSize({windowWidth * windowWidthScale, windowHeight * windowHeightScale});
            depthTextureDescriptor->setWidth(windowWidth * windowWidthScale);
            depthTextureDescriptor->setHeight(windowHeight * windowHeightScale);
            depthBuffer->release();
            depthBuffer = metalDevice->newTexture(depthTextureDescriptor);
        }
        prevWindowWidth = windowWidth;
        prevWindowHeight = windowHeight;

        // Handle input
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            position.z += movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            position.z -= movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            position.x -= movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            position.x += movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            position.y -= movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            position.y += movementSpeed;
        }
        glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            Math::Vector2d mouseDelta = prevMousePos - mousePos;
            yawPitchRoll.x += sensitivity * float(mouseDelta.x);
            yawPitchRoll.y += sensitivity * float(Math::Clamp(mouseDelta.y, -89.5, 89.5));
        }
        prevMousePos = mousePos;

        // Prepare for drawing next frame
        auto metalDrawable = metalLayer->nextDrawable();
        auto metalRenderPassDesc = MTL::RenderPassDescriptor::alloc()->init();
        auto metalColourAttachment = metalRenderPassDesc->colorAttachments()->object(0);
        metalColourAttachment->setClearColor(MTL::ClearColor::Make(0.2, 0.8, 0.8, 1.0));
        metalColourAttachment->setLoadAction(MTL::LoadActionClear);
        metalColourAttachment->setStoreAction(MTL::StoreActionStore);
        metalColourAttachment->setTexture(metalDrawable->texture());
        auto metalDepthAttachment = metalRenderPassDesc->depthAttachment();
        metalDepthAttachment->setTexture(depthBuffer);
        metalDepthAttachment->setClearDepth(1.0);
        metalDepthAttachment->setStoreAction(MTL::StoreActionDontCare);

        auto metalCommandBuffer = metalCommandQueue->commandBuffer();
        auto metalCommandEncoder = metalCommandBuffer->renderCommandEncoder(metalRenderPassDesc);

        // ImGui related
        /*
        ImGui_ImplMetal_NewFrame(metalRenderPassDesc);
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (imguiShowDemo)
        {
            ImGui::ShowDemoWindow(&imguiShowDemo);
        }

        ImGui::Begin("Image Window!", nullptr, ImGuiWindowFlags_NoScrollbar);
        ImVec2 imageSize = ImGui::GetContentRegionAvail();
        float imageAspectRatio = (float)imageWidth / (float)imageHeight;
        float imageWindowRatio = imageSize.x / imageSize.y;
        if (imageAspectRatio > imageWindowRatio) imageSize.y = imageSize.x / imageAspectRatio;
        else imageSize.x = imageSize.y * imageAspectRatio;
        ImGui::Image(metalTexture, imageSize);
        ImGui::End();
        /**/

        // Update camera and cube
        Camera *camera = reinterpret_cast<Camera *>(cameraData->contents());
        camera->projection = Math::Transform::PerspectiveProjection(Math::ToRadians(90.0f), float(windowWidth) / float(windowHeight), 0.1f, 1000.0f);
        camera->view       = Math::Quaternionf::MakeFromYawPitchRoll(yawPitchRoll.x, yawPitchRoll.y, yawPitchRoll.z).ToMatrix4()
                           * Math::Transform::Translate(-position);
        cameraData->didModifyRange(NS::Range::Make(0, sizeof(Camera)));

        Instance *instance = reinterpret_cast<Instance *>(cubeInstances->contents());
        instance->transform = Math::Matrix4f(1.0f);
        cubeInstances->didModifyRange(NS::Range::Make(0, sizeof(Instance)));

        // Custom rendering goes here
        metalCommandEncoder->setRenderPipelineState(pipelineState);
        metalCommandEncoder->setDepthStencilState(depthStencilState);

        metalCommandEncoder->setVertexBuffer(cubeVertices, 0, 0);
        metalCommandEncoder->setVertexBuffer(cubeInstances, 0, 1);
        metalCommandEncoder->setVertexBuffer(cameraData, 0, 2);

        metalCommandEncoder->setCullMode(MTL::CullModeNone);
        metalCommandEncoder->setFrontFacingWinding(MTL::Winding::WindingCounterClockwise);

        metalCommandEncoder->drawIndexedPrimitives(
            MTL::PrimitiveType::PrimitiveTypeTriangle,
            cube.Indices.size(),
            MTL::IndexType::IndexTypeUInt16,
            cubeIndices,
            0,
            1
        );

        // Finish ImGui
        /*
        ImGui::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), metalCommandBuffer, metalCommandEncoder);

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        /**/

        // Finish the current frame
        metalCommandEncoder->endEncoding();
        metalCommandBuffer->presentDrawable(metalDrawable);
        metalCommandBuffer->commit();

        metalCommandEncoder->release();
        metalCommandBuffer->release();
        metalRenderPassDesc->release();
        metalDrawable->release();
    }

    cubeVertices->release();
    cubeIndices->release();
    cubeInstances->release();
    cameraData->release();
    shaderLibrary->release();
    depthStencilState->release();
    depthTextureDescriptor->release();
    depthBuffer->release();

    metalTextureDescriptor->release();
    metalTexture->release();
    metalDevice->release();
    metalCommandQueue->release();

    glfwDestroyWindow(window);

    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();
}
