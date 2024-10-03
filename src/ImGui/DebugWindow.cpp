#include "pch.h"
#include "DebugWindow.h"
#include <imgui.h>
#include <imgui_internal.h>
#include "Renderer/Renderer.h"
#include "Asset/Manager.h"
#include "Core/Game.h"
#include "Sound/SoundEngine.h"

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


        m_WindowActive = ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse;

        ImGui::Begin("Debug");
        m_WindowActive = ImGui::IsWindowFocused();

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

        ImGui::Text("Audio settings");
        float curr_volume = SoundEngine::GetVolume();
        if (ImGui::SliderFloat("Volume", &curr_volume, 0.0f, 1.f)) { SoundEngine::SetVolume(curr_volume); }

        if (ImGui::Button("Stop all sounds")) { SoundEngine::StopAllAudio(); }

        ImGui::End();

        ImGui::Begin("Stat");
        ImGui::Text("Scene stats:");
        ImGui::Text("	- Entity count: %i", current_scene->EntitySize());

        ImGui::Text("Renderer stats:");
        ImGui::Text("	- FPS: %f", frame_stats.FPS);
        ImGui::Text("	- Quads drawn: %i", Renderer::GetStats().QuadsDrawn);
        ImGui::Text("	- Draw calls: %i", Renderer::GetStats().DrawCalls);
        ImGui::End();


        ImGui::Begin("Asset manager");
        ImGui::Text("Asset count count: %i", AssetManager::GetGlobalHeader().NumAssets);
        ImGui::Text("Assets:");
        for (auto &[handle, toc] : AssetManager::GetTOCEntries()) {
            if (ImGui::TreeNode(handle.c_str())) {
                std::string uuid = fmt::format("UUID: {}", toc.UUID);
                std::string length = fmt::format("Length: {}", toc.Length);
                std::string offset = fmt::format("Offset: {}", toc.Offset);
                std::string type = fmt::format("Asset Type: {}", AssetTypeToString((AssetType)toc.Type));

                ImGui::Text(uuid.c_str());
                ImGui::Text(length.c_str());
                ImGui::Text(offset.c_str());
                ImGui::Text(type.c_str());

                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

}// namespace SOF
