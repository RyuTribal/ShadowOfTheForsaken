#include "pch.h"
#include "Engine/ImGui/ImGuiLayer.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>


#include "Engine/Core/Game.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace SOF
{

    struct ImGuiGlobals
    {
        bool BlockEvents = true;
    };

    ImGuiGlobals *s_Data = nullptr;

    void ImGuiLayer::Init()
    {
        IMGUI_CHECKVERSION();
        s_Data = new ImGuiGlobals();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        /*io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;*/

        ImGui::StyleColorsDark();

        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Game *game = Game::Get();
        GLFWwindow *window = static_cast<GLFWwindow *>(game->GetWindow().GetNativeWindow());

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        game->GetRenderingThread().Run(ImGui_ImplOpenGL3_Init, "#version 410");
    }

    void ImGuiLayer::Shutdown()
    {

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        delete s_Data;
        s_Data = nullptr;
    }

    void ImGuiLayer::OnEvent(Event &e)
    {
        if (s_Data && s_Data->BlockEvents) {
            ImGuiIO &io = ImGui::GetIO();
            e.Handled |= e.IsInCategory(EventCategoryMouse) && io.WantCaptureMouse;
            e.Handled |= e.IsInCategory(EventCategoryKeyboard) && io.WantCaptureKeyboard;
        }
    }

    void ImGuiLayer::Begin()
    {
        Game *game = Game::Get();
        Thread<RenderBufferData> &renderer_thread = game->GetRenderingThread();
        renderer_thread.Run(ImGui_ImplOpenGL3_NewFrame);
        renderer_thread.WaitForAllTasks();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::End()
    {
        ImGuiIO &io = ImGui::GetIO();
        Game *game = Game::Get();
        Thread<RenderBufferData> &renderer_thread = game->GetRenderingThread();

        io.DisplaySize = ImVec2((float)(game->GetWindow().GetWidth()), (float)game->GetWindow().GetHeight());


        ImGui::Render();
        renderer_thread.Run(ImGui_ImplOpenGL3_RenderDrawData, ImGui::GetDrawData());
        renderer_thread.WaitForAllTasks();

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {


            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();

            renderer_thread.Run([&]() {
                auto context = Renderer::GetContext()->Handle();
                glfwMakeContextCurrent(context);
            });
            renderer_thread.WaitForAllTasks();
        }
    }


    void ImGuiLayer::BlockEvents(bool block)
    {
        if (s_Data) { s_Data->BlockEvents = block; }
    }

}// namespace SOF
