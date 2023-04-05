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

struct Camera
{
    Math::Matrix4f view;
    Math::Matrix4f projection;
};

struct Instance
{
    Math::Matrix4f transform;
};

Math::Vector3f ToVec3(const Math::Vector4f& vec4)
{
    return Math::Vector3f(vec4.x, vec4.y, vec4.z);
}

int main(int argc, char **argv)
{
    int32_t prevWindowWidth = 640;
    int32_t prevWindowHeight = 480;
    int32_t windowWidth = 640;
    int32_t windowHeight = 480;

    float movementSpeed = 0.05f;
    float sensitivity = 0.5f;
    Math::Vector2d prevMousePos;
    Math::Vector2d mousePos;
    Math::Matrix4f cameraTransform(1.0f);
    float yaw = 0.0f;
    float pitch = 0.0f;
    Math::Vector3f position;

    std::cout << "Hello world!\n";

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

    // Allocate (and fill some of) GPU memory buffers
    auto cube = Utils::Align(Utils::MakeCube());
    MTL::Buffer *cubeVertices
        = metalDevice->newBuffer(cube.Vertices.size() * sizeof(Utils::AlignedVertex), MTL::ResourceStorageModeManaged);
    MTL::Buffer *cubeIndices
        = metalDevice->newBuffer(cube.Indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeManaged);

    memcpy(cubeVertices->contents(), &(cube.Vertices[0]), cube.Vertices.size() * sizeof(Utils::AlignedVertex));
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
        Math::Vector3f positionDelta;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            positionDelta.z += movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            positionDelta.z -= movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            positionDelta.x -= movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            positionDelta.x += movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            positionDelta.y -= movementSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            positionDelta.y += movementSpeed;
        }
        glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
        position += ToVec3(Math::Transform::RotateByY(-yaw) * Math::Transform::RotateByX(-pitch) * Math::Vector4f(positionDelta));
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            Math::Vector2d mouseDelta = prevMousePos - mousePos;
            yaw += std::remainder(Math::ToRadians(sensitivity * float(mouseDelta.x)), 2 * Math::Constants::Pi_f);
            pitch = Math::Clamp(pitch + Math::ToRadians(sensitivity * float(mouseDelta.y)), Math::ToRadians(-89.5f), Math::ToRadians(89.5f));
        }
        prevMousePos = mousePos;
        cameraTransform = Math::Transform::RotateByX(pitch) * Math::Transform::RotateByY(yaw) * Math::Transform::Translate(-position);  

        // Prepare for drawing next frame
        auto metalDrawable = metalLayer->nextDrawable();
        auto metalRenderPassDesc = MTL::RenderPassDescriptor::alloc()->init();
        auto metalColourAttachment = metalRenderPassDesc->colorAttachments()->object(0);
        metalColourAttachment->setClearColor(MTL::ClearColor::Make(0.2, 0.8, 0.8, 1.0));
        metalColourAttachment->setLoadAction(MTL::LoadActionClear);
        metalColourAttachment->setStoreAction(MTL::StoreActionStore);
        metalColourAttachment->setTexture(metalDrawable->texture());
        auto metalDepthAttachment = metalRenderPassDesc->depthAttachment();
        metalDepthAttachment->setClearDepth(1.0);
        metalDepthAttachment->setStoreAction(MTL::StoreActionDontCare);
        metalDepthAttachment->setTexture(depthBuffer);

        auto metalCommandBuffer = metalCommandQueue->commandBuffer();
        auto metalCommandEncoder = metalCommandBuffer->renderCommandEncoder(metalRenderPassDesc);

        // Update camera and cube
        Camera *camera = reinterpret_cast<Camera *>(cameraData->contents());
        camera->projection = Math::Transpose(Math::Transform::PerspectiveProjection(Math::ToRadians(70.0f), float(windowWidth) / float(windowHeight), 0.1f, 1000.0f));
        camera->view       = Math::Transpose(cameraTransform);
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

        metalCommandEncoder->setCullMode(MTL::CullModeBack);
        metalCommandEncoder->setFrontFacingWinding(MTL::Winding::WindingCounterClockwise);

        metalCommandEncoder->drawIndexedPrimitives(
            MTL::PrimitiveType::PrimitiveTypeTriangle,
            cube.Indices.size(),
            MTL::IndexType::IndexTypeUInt16,
            cubeIndices,
            0,
            1
        );

        // ImGui related
        ImGui_ImplMetal_NewFrame(metalRenderPassDesc);
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Info");

        ImGui::Text("Camera info");
        ImGui::Text("Position: %f, %f, %f", position.x, position.y, position.z);
        ImGui::Text("Yaw: %f, Pitch: %f", yaw, pitch);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), metalCommandBuffer, metalCommandEncoder);

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

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

    metalDevice->release();
    metalCommandQueue->release();

    glfwDestroyWindow(window);

    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();
}
