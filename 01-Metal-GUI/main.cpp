#include <iostream>
#include <cstdint>

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

#include <stb_image.h>

#include "metalUtil.h"

int main(int argc, char **argv)
{
    int32_t windowWidth = 640;
    int32_t windowHeight = 480;

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

    // Load a texture into memory
    int32_t imageWidth, imageHeight, imageChannels;
    uint8_t *imageData = stbi_load("116.jpeg", &imageWidth, &imageHeight, &imageChannels, 4);
    MTL::TextureDescriptor *metalTextureDescriptor =
        MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormatRGBA8Unorm,
                                                    imageWidth,
                                                    imageHeight,
                                                    false);
    metalTextureDescriptor->setUsage(MTL::TextureUsageShaderRead);
    MTL::Texture *metalTexture = metalDevice->newTexture(metalTextureDescriptor);
    metalTexture->replaceRegion({0, 0, 0, (uint32_t)imageWidth, (uint32_t)imageHeight, 1}, 0, imageData, imageWidth * 4);
    stbi_image_free(imageData);

    bool done = false;
    while(!done)
    {
        // Update the window and layer size
        glfwPollEvents();
        done = glfwWindowShouldClose(window);
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwGetWindowContentScale(window, &windowWidthScale, &windowHeightScale);
        metalLayer->setDrawableSize({windowWidth * windowWidthScale, windowHeight * windowHeightScale});

        // Prepare for drawing next frame
        auto metalDrawable = metalLayer->nextDrawable();
        auto metalRenderPassDesc = MTL::RenderPassDescriptor::alloc()->init();
        auto metalColourAttachment = metalRenderPassDesc->colorAttachments()->object(0);
        metalColourAttachment->setClearColor(MTL::ClearColor::Make(1.0, 0.0, 0.0, 1.0));
        metalColourAttachment->setLoadAction(MTL::LoadActionClear);
        metalColourAttachment->setStoreAction(MTL::StoreActionStore);
        metalColourAttachment->setTexture(metalDrawable->texture());

        auto metalCommandBuffer = metalCommandQueue->commandBuffer();
        auto metalCommandEncoder = metalCommandBuffer->renderCommandEncoder(metalRenderPassDesc);

        // ImGui related
        ImGui_ImplMetal_NewFrame(metalRenderPassDesc);
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (imguiShowDemo)
        {
            ImGui::ShowDemoWindow(&imguiShowDemo);
        }

        ImGui::Begin("Image Window!");
        ImVec2 imageSize = ImGui::GetContentRegionAvail();
        float imageAspectRatio = (float)imageWidth / (float)imageHeight;
        float imageWindowRatio = imageSize.x / imageSize.y;
        if (imageAspectRatio > imageWindowRatio) imageSize.y *= imageWindowRatio;
        else imageSize.x /= imageWindowRatio;
        ImGui::Image(metalTexture, imageSize);
        ImGui::End();

        // Custom rendering goes here
        // TODO

        // Finish up current ImGui frame
        ImGui::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), metalCommandBuffer, metalCommandEncoder);

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // Finish the current frame
        metalCommandEncoder->endEncoding();
        metalCommandBuffer->presentDrawable(metalDrawable);
        metalCommandBuffer->commit();

        metalCommandEncoder->release();
        metalCommandBuffer->release();
        metalRenderPassDesc->release();
        metalDrawable->release();
    }

    metalTextureDescriptor->release();
    metalTexture->release();
    metalDevice->release();
    metalCommandQueue->release();

    glfwDestroyWindow(window);

    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();
}
