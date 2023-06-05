#include <iostream>
#include <cstdint>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define IMGUI_IMPL_METAL_CPP
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#include <stb_image.h>

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Init GL loader
    int version = gladLoadGL(glfwGetProcAddress);
    if(!version)
    {
        std::cout << "Failed to initialize OpenGL context\n";
        return -1;
    }
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << "\n";

    // Prepare the viewport
    glViewport(0, 0, windowWidth, windowHeight);

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGuiContext *imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable
                               |  ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    bool imguiShowDemo = true;

    // Load a texture into memory
    int imageWidth, imageHeight, imageChannels;
    uint8_t *imageData = stbi_load("116.jpeg", &imageWidth, &imageHeight, &imageChannels, 4);

    uint32_t openglTexture;
    glGenTextures(1, &openglTexture);
    glBindTexture(GL_TEXTURE_2D, openglTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    stbi_image_free(imageData);

    bool done = false;
    while(!done)
    {
        // Update the window and viewport size
        glfwPollEvents();
        done = glfwWindowShouldClose(window);
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        // Prepare for drawing the next frame
        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // ImGui related
        ImGui_ImplOpenGL3_NewFrame();
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
        if (imageAspectRatio > imageWindowRatio) imageSize.y = imageSize.x / imageAspectRatio;
        else imageSize.x = imageSize.y * imageAspectRatio;
        ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(openglTexture)), imageSize);
        ImGui::End();


        // Custom rendering goes here
        // TODO

        // Finish up current ImGui frame
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // Finish current frame
        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &openglTexture);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

