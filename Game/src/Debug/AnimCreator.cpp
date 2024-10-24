#include "pch.h"
#include "AnimCreator.h"
#include "Animation/Animation.h"
#include "SOF/Game.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace SOF
{
    AnimCreator *s_Instance = nullptr;

    AnimCreator::AnimCreator() {}

    void AnimCreator::Init() { s_Instance = new AnimCreator(); }

    void AnimCreator::Shutdown()
    {
        delete s_Instance;
        s_Instance = nullptr;
    }

    void AnimCreator::Render(float dt)
    {
        SOF_ASSERT(s_Instance, "No instance available");
        auto toc_table = AssetManager::GetTOCEntries();
        std::vector<std::string> sprite_sheets;
        std::vector<std::string> animation_handles;

        for (auto &[handle, toc] : toc_table) {
            std::string asset_type = AssetManager::ExtractMetadataVariable(toc.MetaData, "AssetType");
            if ((AssetType)toc.Type == AssetType::Texture) {
                sprite_sheets.push_back(handle);
            } else if (asset_type != "" && asset_type == "Animation") {
                animation_handles.push_back(handle);
            }
        }
        const char *combo_label = s_Instance->m_SpriteSheet ? s_Instance->m_SpriteSheetHandle.c_str() : "None";
        ImGui::Begin("Animation creator");
        if (ImGui::Button("Load animation")) { ImGui::OpenPopup("anim_load_popup"); }
        if (ImGui::BeginCombo("Sprite Sheets", combo_label)) {
            for (const auto &sprite_sheet : sprite_sheets) {
                bool isSelected = (s_Instance->m_SpriteSheetHandle == sprite_sheet);
                if (ImGui::Selectable(sprite_sheet.c_str(), isSelected)) {
                    s_Instance->m_SpriteSheet = AssetManager::Load<Texture>(sprite_sheet);
                    s_Instance->m_SpriteSheetHandle = sprite_sheet;
                }
                if (isSelected) { ImGui::SetItemDefaultFocus(); }
            }
            ImGui::EndCombo();
        }


        if (s_Instance->m_SpriteSheet) {
            s_Instance->m_CurrFrameTime += dt;
            if (s_Instance->m_CurrFrameTime >= s_Instance->m_FrameTime) {
                s_Instance->m_CurrFrameTime = 0.0f;
                s_Instance->m_CurrentAnimatedFrame += 1;
            }
            if (s_Instance->m_CurrentAnimatedFrame >= s_Instance->m_AnimationIndices.size()) {
                s_Instance->m_CurrentAnimatedFrame = 0;
            }

            ImGui::Text("Spritesheet dimensions %dx%d",

              s_Instance->m_SpriteSheet->GetWidth(),
              s_Instance->m_SpriteSheet->GetHeight());

            std::vector<char> buffer(s_Instance->m_AnimationHandle.begin(), s_Instance->m_AnimationHandle.end());
            buffer.resize(256);
            if (ImGui::InputText("Animation handle", buffer.data(), buffer.size())) {
                s_Instance->m_AnimationHandle = std::string(buffer.data());
            }

            ImGui::InputInt("Sprite width", (int *)&s_Instance->m_SpriteWidth);
            ImGui::InputInt("Sprite height", (int *)&s_Instance->m_SpriteHeight);
            ImGui::DragFloat("Frame time (in seconds)", &s_Instance->m_FrameTime, 0.001f, 0.0f);

            ImTextureID texture = (ImTextureID)(uintptr_t)s_Instance->m_SpriteSheet->GetRendererID();
            float image_width = std::min(ImGui::GetWindowWidth(), 500.f);
            float percentage_size = image_width / (float)s_Instance->m_SpriteSheet->GetWidth();
            float image_height = percentage_size * (float)s_Instance->m_SpriteSheet->GetHeight();

            ImVec2 image_size(image_width, image_height);
            ImVec2 sprite_size(
              percentage_size * s_Instance->m_SpriteWidth, percentage_size * s_Instance->m_SpriteHeight);

            ImVec2 image_pos = ImGui::GetCursorScreenPos();
            ImGui::Image(texture, image_size, ImVec2(0, 1), ImVec2(1, 0));

            ImVec2 mouse_pos = ImGui::GetMousePos();
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                float relative_mouse_x = mouse_pos.x - image_pos.x;
                float relative_mouse_y = mouse_pos.y - image_pos.y;

                if (relative_mouse_x >= 0 && relative_mouse_x <= image_size.x && relative_mouse_y >= 0
                    && relative_mouse_y <= image_size.y) {
                    uint32_t sprite_x = static_cast<uint32_t>(relative_mouse_x / sprite_size.x);
                    uint32_t sprite_y = static_cast<uint32_t>(relative_mouse_y / sprite_size.y);

                    uint32_t inverted_sprite_y =
                      (s_Instance->m_SpriteSheet->GetHeight() / s_Instance->m_SpriteHeight) - 1 - sprite_y;

                    if (sprite_x < s_Instance->m_SpriteSheet->GetWidth() / s_Instance->m_SpriteWidth
                        && sprite_y < s_Instance->m_SpriteSheet->GetHeight() / s_Instance->m_SpriteHeight) {
                        s_Instance->m_SelectedIndex = { sprite_x, inverted_sprite_y };
                    }
                }
            }

            if (s_Instance->m_SelectedIndex.first != -1 && s_Instance->m_SelectedIndex.second != -1) {
                ImDrawList *draw_list = ImGui::GetWindowDrawList();
                ImVec2 p0 = ImVec2(image_pos.x + s_Instance->m_SelectedIndex.first * sprite_size.x,
                  image_pos.y
                    + (s_Instance->m_SpriteSheet->GetHeight() / std::max(s_Instance->m_SpriteHeight, uint32_t(1))
                        - s_Instance->m_SelectedIndex.second - 1)
                        * std::max(sprite_size.y, 1.f));

                ImVec2 p1 = ImVec2(p0.x + sprite_size.x, p0.y + sprite_size.y);

                draw_list->AddRect(p0, p1, IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
            }

            if (ImGui::Button("Create new frame")) {
                s_Instance->m_AnimationIndices.push_back(s_Instance->m_SelectedIndex);
                s_Instance->m_FullDuration = s_Instance->m_FrameTime * (float)s_Instance->m_AnimationIndices.size();
            }

            ImGui::BeginGroup();
            ImGui::Text("Frames:");
            for (size_t i = 0; i < s_Instance->m_AnimationIndices.size(); ++i) {
                std::pair<uint32_t, uint32_t> index = s_Instance->m_AnimationIndices[i];

                ImGui::BeginGroup();

                float u_min_x =
                  (index.first * s_Instance->m_SpriteWidth) / (float)s_Instance->m_SpriteSheet->GetWidth();
                float u_max_x =
                  ((index.first + 1) * s_Instance->m_SpriteWidth) / (float)s_Instance->m_SpriteSheet->GetWidth();

                float u_min_y =
                  (index.second * s_Instance->m_SpriteHeight) / (float)s_Instance->m_SpriteSheet->GetHeight();
                float u_max_y =
                  ((index.second + 1) * s_Instance->m_SpriteHeight) / (float)s_Instance->m_SpriteSheet->GetHeight();

                ImGui::Image((ImTextureID)(uintptr_t)s_Instance->m_SpriteSheet->GetRendererID(),
                  ImVec2(50, 50),
                  ImVec2(u_min_x, u_max_y),
                  ImVec2(u_max_x, u_min_y));

                if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
                    s_Instance->m_AnimationIndices.erase(s_Instance->m_AnimationIndices.begin() + i);
                    s_Instance->m_CurrFrameTime = 0.0f;
                    s_Instance->m_CurrentAnimatedFrame = 0;
                }
                ImGui::EndGroup();

                if (i < s_Instance->m_AnimationIndices.size() - 1 && i % 10 != 9) { ImGui::SameLine(); }
            }
            ImGui::EndGroup();

            if (s_Instance->m_AnimationIndices.size() > 0) {
                ImGui::Text("Preview:");
                std::pair<uint32_t, uint32_t> index =
                  s_Instance->m_AnimationIndices[s_Instance->m_CurrentAnimatedFrame];

                float u_min_x =
                  (index.first * s_Instance->m_SpriteWidth) / (float)s_Instance->m_SpriteSheet->GetWidth();
                float u_max_x =
                  ((index.first + 1) * s_Instance->m_SpriteWidth) / (float)s_Instance->m_SpriteSheet->GetWidth();

                float u_min_y =
                  (index.second * s_Instance->m_SpriteHeight) / (float)s_Instance->m_SpriteSheet->GetHeight();
                float u_max_y =
                  ((index.second + 1) * s_Instance->m_SpriteHeight) / (float)s_Instance->m_SpriteSheet->GetHeight();

                ImGui::Image((ImTextureID)(uintptr_t)s_Instance->m_SpriteSheet->GetRendererID(),
                  ImVec2(100, 100),
                  ImVec2(u_min_x, u_max_y),
                  ImVec2(u_max_x, u_min_y));
            }

            if (s_Instance->m_AnimationHandle == "") {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }
            if (ImGui::Button("Save animation")) {
                s_Instance->m_IsSaving = true;
                s_Instance->m_IsOverwritting = AssetManager::Exists(s_Instance->m_AnimationHandle);
            }

            if (s_Instance->m_AnimationHandle == "") {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

        } else {
            ImGui::Text("No spritesheet selected");
        }


        if (ImGui::BeginPopup("anim_load_popup")) {
            ImGui::Text("Load Asset:");
            static size_t selected_anim = 0;
            const char *combo_label = animation_handles[selected_anim].c_str();
            if (ImGui::BeginCombo("Animation assets", combo_label)) {
                for (size_t i = 0; i < animation_handles.size(); i++) {
                    if (ImGui::Selectable(animation_handles[i].c_str(), i == selected_anim)) { selected_anim = i; }
                    if (i == selected_anim) { ImGui::SetItemDefaultFocus(); }
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("Load")) {
                auto animation_asset = AssetManager::Load<Animation>(animation_handles[selected_anim]);
                auto toc = AssetManager::GetTOCEntries()[animation_handles[selected_anim]];
                s_Instance->m_AnimationHandle = animation_handles[selected_anim];
                s_Instance->m_FrameTime = animation_asset->GetFrameDuration();
                s_Instance->m_AnimationIndices = animation_asset->GetFrames();
                s_Instance->m_SelectedIndex = s_Instance->m_AnimationIndices[0];
                s_Instance->m_CurrentAnimatedFrame = 0;
                s_Instance->m_CurrFrameTime = 0.0f;
                s_Instance->m_SpriteWidth =
                  std::stoi(AssetManager::ExtractMetadataVariable(toc.MetaData, "SpriteWidth"));
                s_Instance->m_SpriteHeight =
                  std::stoi(AssetManager::ExtractMetadataVariable(toc.MetaData, "SpriteHeight"));
                s_Instance->m_SpriteSheetHandle =
                  AssetManager::ExtractMetadataVariable(toc.MetaData, "SpriteSheetHandle");
                s_Instance->m_SpriteSheet = AssetManager::Load<Texture>(s_Instance->m_SpriteSheetHandle);

                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
        ImGui::End();

        if (ImGui::BeginPopup("anim_overwrite_popup")) {
            ImGui::Text("Asset with the same handle found! Overwrite existing asset?");
            if (ImGui::Button("Yes")) {
                s_Instance->m_IsOverwritting = false;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("No")) {
                s_Instance->m_IsOverwritting = false;
                s_Instance->m_IsSaving = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (s_Instance->m_IsSaving) {

            if (s_Instance->m_IsOverwritting) {
                ImGui::OpenPopup("anim_overwrite_popup");
            } else {
                AnimationSerializer::Animation anim_data;
                anim_data.FrameCount = s_Instance->m_AnimationIndices.size();
                anim_data.Frames = s_Instance->m_AnimationIndices;
                anim_data.FrameTime = s_Instance->m_FrameTime;
                anim_data.SpriteHeight = s_Instance->m_SpriteHeight;
                anim_data.SpriteWidth = s_Instance->m_SpriteWidth;
                memcpy(anim_data.SpriteSheetHandle,
                  s_Instance->m_SpriteSheetHandle.c_str(),
                  s_Instance->m_SpriteSheetHandle.size() + 1 * sizeof(char));

                auto data = AnimationSerializer::DumpData(anim_data);
                AssetManager::DeregisterAsset(s_Instance->m_AnimationHandle, true);

                AssetManager::RegisterAssetFromMemory(
                  data, s_Instance->m_AnimationHandle, AssetType::Custom, data->MetaData["AssetType"]);

                s_Instance->m_AnimationIndices.clear();
                s_Instance->m_AnimationHandle = "";
                s_Instance->m_IsSaving = false;
            }
        }
    }

}// namespace SOF
