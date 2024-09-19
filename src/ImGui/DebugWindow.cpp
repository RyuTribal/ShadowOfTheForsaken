#include "pch.h"
#include "DebugWindow.h"
#include <imgui.h>
#include "Renderer/Renderer.h"
#include "Asset/Manager.h"
#include "Core/Game.h"

namespace SOF
{
    DebugWindow::DebugWindow()
    {
        m_AssetFilePath[0] = 0;
        m_AssetHandle[0] = 0;
        m_DeletedAssetHandle[0] = 0;
    }

    void DebugWindow::Render()
    {
        Scene *current_scene = Game::Get()->GetCurrentScene();
        FrameStats &frame_stats = Game::Get()->GetFrameStats();

        ImGui::Begin("Debug");

        if (ImGui::ColorEdit3("Background color", m_BgColor)) {
            glm::vec3 color_vec(m_BgColor[0], m_BgColor[1], m_BgColor[2]);
            Renderer::ChangeBackgroundColor(color_vec);
        }


        ImGui::Text("Register asset into the asset pack:");
        ImGui::InputText("File path", m_AssetFilePath, 255);
        ImGui::InputText("Asset handle##register", m_AssetHandle, 255);
        if (ImGui::Button("Register asset") && m_AssetHandle[0] != 0 && m_AssetFilePath[0] != 0) {
            std::string handle = std::string(m_AssetHandle);
            AssetManager::RegisterAsset(m_AssetFilePath, handle, AssetManager::FileToAssetType(m_AssetFilePath));
            memset(m_AssetFilePath, 0, sizeof(m_AssetFilePath));
            memset(m_AssetHandle, 0, sizeof(m_AssetHandle));
        }

        ImGui::Text("Deregister asset from the asset pack:");
        ImGui::InputText("Asset handle##deregister", m_DeletedAssetHandle, 255);
        if (ImGui::Button("Deregister asset") && m_DeletedAssetHandle[0] != 0) {
            std::string handle = std::string(m_DeletedAssetHandle);
            AssetManager::DeregisterAsset(handle);
            memset(m_DeletedAssetHandle, 0, sizeof(m_DeletedAssetHandle));
        }

        ImGui::End();

        ImGui::Begin("Stat");
        ImGui::Text("Scene stats:");
        ImGui::Text("	- Entity count: %i", current_scene->EntitySize());

        ImGui::Text("Renderer stats:");
        ImGui::Text("	- FPS: %f", frame_stats.FPS);
        ImGui::Text("	- Quads drawn: %i", Renderer::GetStats().QuadsDrawn);
        ImGui::Text("	- Draw calls: %i", Renderer::GetStats().DrawCalls);
        ImGui::End();
    }

}// namespace SOF
