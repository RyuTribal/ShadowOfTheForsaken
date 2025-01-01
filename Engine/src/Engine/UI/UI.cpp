#include "pch.h"
#include "UI.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Asset/Manager.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Game.h"
#include <glm/ext/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace SOF
{
#define NUM_CHARS 96// from 32 to 127


    UI *UI::s_Instance = nullptr;

    void UI::Init()
    {
        s_Instance = new UI();
        FT_Error error = FT_Init_FreeType(&s_Instance->m_Library);
        SOF_ASSERT(!error, "An error occured initializing freetype");
        RegisterFont("assets/fonts/depixel/DePixelBreit.ttf");
    }

    void UI::Shutdown()
    {
        delete s_Instance;
        s_Instance = nullptr;
    }

    void UI::BeginFrame()
    {
        SOF_ASSERT(s_Instance, "No UI instance detected, did you forget to run Init()?");

        s_Instance->m_Canvases.clear();
        for (auto &cache : s_Instance->m_CanvasStateCache) { cache.second->Elements.clear(); }
        s_Instance->m_CurrentActiveCanvas = nullptr;
        s_Instance->m_CapturingMouse = false;
        s_Instance->m_CapturingInput = false;
    }

    void UI::EndFrame()
    {
        SOF_ASSERT(s_Instance, "No UI instance detected, did you forget to run Init()?");
        std::unordered_map<std::string, std::shared_ptr<UICanvas>> newCache;
        for (auto &window : s_Instance->m_Canvases) { newCache[window->ID] = window; }
        s_Instance->m_CanvasStateCache.swap(newCache);
        std::sort(s_Instance->m_Canvases.begin(),
          s_Instance->m_Canvases.end(),
          [](const std::shared_ptr<UICanvas> &a, const std::shared_ptr<UICanvas> &b) { return a->ZOrder < b->ZOrder; });

        uint32_t layer = UILayerOffset;

        for (auto &canvas : s_Instance->m_Canvases) {
            canvas->Layer = layer++;
            glm::vec2 normalized_scale = NormalizeScreenScale(canvas->Scale);
            glm::vec2 normalized_pos = NormalizeScreenPosition(canvas->Position, normalized_scale);
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(normalized_pos, 0.0f));
            transform = glm::scale(transform, glm::vec3(normalized_scale, 1.0f));

            SpriteData sprite_data;
            sprite_data.Transform = transform;
            sprite_data.Color = canvas->BackgroundColor;
            sprite_data.Layer = canvas->Layer;
            sprite_data.ShaderHandle = "ui";
            Renderer::SubmitSquare(&sprite_data);
            float next_offset = 0.0f;
            for (auto element : canvas->Elements) {
                next_offset =
                  element->Draw(transform, canvas->Position, canvas->Scale, sprite_data.Layer, { 0.f, next_offset }).y;
            }
        }
    }

    bool UI::IsCapturingMouse()
    {
        SOF_ASSERT(s_Instance, "No UI instance detected, did you forget to run Init()?");
        return s_Instance->m_CapturingMouse;
    }

    bool UI::IsCapturingInput()
    {
        SOF_ASSERT(s_Instance, "No UI instance detected, did you forget to run Init()?");
        return s_Instance->m_CapturingInput;
    }

    bool UI::OnKeyPress(KeyPressedEvent &event) { return false; }

    bool UI::OnKeyReleased(KeyReleasedEvent &event) { return false; }

    bool UI::OnMouseButtonPress(MouseButtonPressedEvent &event)
    {
        SOF_ASSERT(s_Instance, "No UI instance detected, did you forget to run Init()?");

        glm::vec2 screen_multiplier = { UIBaseResolution.x / (float)Game::Get()->GetWindow().GetWidth(),
            UIBaseResolution.y / (float)Game::Get()->GetWindow().GetHeight() };
        glm::vec2 mouse_position = Input::GetMousePosition();
        mouse_position.x *= screen_multiplier.x;
        mouse_position.y *= screen_multiplier.y;
        for (auto it = s_Instance->m_Canvases.rbegin(); it != s_Instance->m_Canvases.rend(); ++it) {
            auto &canvas = *it;

            glm::vec2 min_bounds = canvas->Position;
            glm::vec2 max_bounds = canvas->Position + canvas->Scale;
            if (mouse_position.x >= min_bounds.x && mouse_position.x <= max_bounds.x && mouse_position.y >= min_bounds.y
                && mouse_position.y <= max_bounds.y) {
                canvas->ZOrder = ++s_Instance->m_NextZOrder;

                s_Instance->m_FocusedCanvas = canvas.get();

                s_Instance->m_CapturingMouse = true;
                s_Instance->m_CapturingInput = true;

                return true;
            }
        }

        return false;
    }

    bool UI::OnMouseMoved(MouseMovedEvent &event)
    {
        SOF_ASSERT(s_Instance, "No UI instance detected, did you forget to run Init()?");

        glm::vec2 mouse_position = Input::GetMousePosition();

        bool handled = false;
        s_Instance->m_CapturingMouse = false;

        for (auto it = s_Instance->m_Canvases.rbegin(); it != s_Instance->m_Canvases.rend(); ++it) {
            auto &window = *it;

            glm::vec2 min_bounds = window->Position;
            glm::vec2 max_bounds = window->Position + window->Scale;
            if (mouse_position.x >= min_bounds.x && mouse_position.x <= max_bounds.x && mouse_position.y >= min_bounds.y
                && mouse_position.y <= max_bounds.y) {
                window->IsHovered = true;
                s_Instance->m_CapturingMouse = true;
                handled = true;
                break;
            } else {
                window->IsHovered = false;
            }
        }

        return handled;
    }

    float UI::ConvertPointsToPixels(float points)
    {
        glm::vec2 scaling = Game::Get()->GetWindow().GetDPI();
        float uniform_scale = std::min(scaling.x, scaling.y);
        float x_scaling = (float)Game::Get()->GetWindow().GetWidth() / UIBaseResolution.x;
        float y_scaling = (float)Game::Get()->GetWindow().GetHeight() / UIBaseResolution.y;
        uniform_scale *= std::min(x_scaling, y_scaling);
        return points * uniform_scale;
    }

    size_t UI::RegisterFont(const std::string &path_to_font, uint32_t font_size)
    {
        SOF_ASSERT(s_Instance, "No UI instance detected, did you forget to run Init()?");

        FT_Face face;
        GlyphInfo glyph_infos[NUM_CHARS];

        SOF_ASSERT(std::filesystem::exists(path_to_font),
        "Font file does not exist: {}",
        path_to_font);

        SOF_ASSERT(!FT_New_Face(s_Instance->m_Library, path_to_font.c_str(), 0, &face),
          "Failed to load the font: {}",
          path_to_font);

        FT_Set_Pixel_Sizes(face, 0, font_size);

        FT_GlyphSlot glyph = face->glyph;

        unsigned int imageWidth = 0;
        unsigned int imageHeight = 0;
        unsigned int maxAscent = 0;
        unsigned int maxDescent = 0;

        for (unsigned int c = 32; c < 127; c++) {
            if (c == 127) continue;
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

            imageWidth += glyph->bitmap.width;

            if (glyph->bitmap_top > (int)maxAscent) { maxAscent = glyph->bitmap_top; }

            if ((glyph->metrics.height >> 6) - glyph->bitmap_top > (int)maxDescent) {
                maxDescent = (glyph->metrics.height >> 6) - glyph->bitmap_top;
            }
            imageHeight = maxAscent + maxDescent;
        }

        unsigned int size = imageWidth * imageHeight;
        unsigned char *buffer = new unsigned char[size]();
        unsigned int xOffset = 0;
        unsigned int yOffset = 0;
        unsigned int maxWidth = 0;

        for (unsigned int c = 32; c < 127; c++) {
            if (c == 127) continue;
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;


            GlyphInfo &glyph_info = glyph_infos[c - 32];
            glyph_info.Advance = glyph->advance.x >> 6;
            glyph_info.BearingX = glyph->bitmap_left;
            glyph_info.BearingY = glyph->bitmap_top;
            glyph_info.Width = glyph->bitmap.width;
            glyph_info.Height = glyph->bitmap.rows;
            glyph_info.U0 = (float)xOffset / (float)imageWidth;
            glyph_info.V0 = 0.0f;
            glyph_info.U1 = (float)(xOffset + glyph->bitmap.width) / (float)imageWidth;
            glyph_info.V1 = 1.0f;

            if (maxWidth < glyph_info.Width) { maxWidth = glyph_info.Width; }

            yOffset = maxAscent - glyph->bitmap_top;

            for (unsigned int x = 0; x < glyph->bitmap.width; x++) {
                for (unsigned int y = 0; y < glyph->bitmap.rows; y++) {
                    unsigned int flipped_y = yOffset + y;
                    unsigned int imageIndex = (x + xOffset) + flipped_y * imageWidth;
                    unsigned int bitmapIndex = x + y * glyph->bitmap.width;
                    buffer[imageIndex] = glyph->bitmap.buffer[bitmapIndex];
                }
            }

            xOffset += glyph->bitmap.width;
        }

        unsigned char *tempRow = new unsigned char[imageWidth];

        for (unsigned int y = 0; y < imageHeight / 2; ++y) {
            unsigned int topIndex = y * imageWidth;
            unsigned int bottomIndex = (imageHeight - y - 1) * imageWidth;
            memcpy(tempRow, buffer + topIndex, imageWidth);
            memcpy(buffer + topIndex, buffer + bottomIndex, imageWidth);
            memcpy(buffer + bottomIndex, tempRow, imageWidth);
        }

        delete[] tempRow;

        auto &rendering_thread = Game::Get()->GetRenderingThread();
        rendering_thread.Run([imageWidth, maxAscent, maxWidth, imageHeight, buffer, font_size, glyph_infos]() {
            TextureSpecification spec;
            spec.Format = ImageFormat::R8;
            spec.Height = imageHeight;
            spec.Width = imageWidth;
            Font font_obj;
            font_obj.FontSize = font_size;
            font_obj.Ascent = maxAscent;
            font_obj.GlyphHeight = imageHeight;
            font_obj.MaxGlyphWidth = maxWidth;
            font_obj.GlyphInfos = std::vector<GlyphInfo>(glyph_infos, glyph_infos + NUM_CHARS);
            font_obj.Atlas = Texture::Create(spec, buffer);
            s_Instance->m_Fonts.push_back(font_obj);
            free(buffer);
        });
        rendering_thread.WaitForAllTasks();
        FT_Done_Face(face);

        return s_Instance->m_Fonts.size() - 1;
    }

    bool UI::OnMouseButtonReleased(MouseButtonReleasedEvent &event) { return false; }

    void UI::OnWindowEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(UI::OnKeyPress);
        dispatcher.Dispatch<KeyReleasedEvent>(UI::OnKeyReleased);
        dispatcher.Dispatch<MouseMovedEvent>(UI::OnMouseMoved);
        dispatcher.Dispatch<MouseButtonPressedEvent>(UI::OnMouseButtonPress);
        dispatcher.Dispatch<MouseButtonReleasedEvent>(UI::OnMouseButtonReleased);
    }

    glm::vec2 UI::ScaleToScreen(const glm::vec2 &vector)
    {
        glm::vec2 screen_size = { Game::Get()->GetWindow().GetWidth(), Game::Get()->GetWindow().GetHeight() };
        glm::vec2 scaling_factor = screen_size / UIBaseResolution;
        return vector * scaling_factor;
    }

    glm::vec2 UI::NormalizeScreenPosition(const glm::vec2 &position, const glm::vec2 &normalized_scale)
    {
        glm::vec2 normalized_pos =
          glm::vec2((2.0f * position.x) / UIBaseResolution.x - 1.0f, 1.0f - (2.0f * position.y) / UIBaseResolution.y);

        normalized_pos = { normalized_pos.x + normalized_scale.x, normalized_pos.y - normalized_scale.y };
        return normalized_pos;
    }

    glm::vec2 UI::NormalizeScreenScale(const glm::vec2 &scale)
    {
        return glm::vec2(scale.x / UIBaseResolution.x, scale.y / UIBaseResolution.y);
    }

    Font *UI::GetFont(size_t index)
    {
        SOF_ASSERT(s_Instance, "No UI instance detected, did you forget to run Init()?");
        SOF_ASSERT(s_Instance->m_Fonts.size() - 1 <= index, "Index is higher than the last registered font");
        return &s_Instance->m_Fonts[index];
    }

    void
      UI::BeginCanvas(const std::string &id, const glm::vec2 &position, const glm::vec2 &scale, const glm::vec4 &color)
    {
        SOF_ASSERT(s_Instance, "No UI instance detected, did you forget to run Init()?");
        SOF_ASSERT(
          !s_Instance->m_CurrentActiveCanvas, "An active canvas is already being prepared. Did you forget EndCanvas?");
        std::shared_ptr<UICanvas> window;
        auto it = s_Instance->m_CanvasStateCache.find(id);
        if (it != s_Instance->m_CanvasStateCache.end()) {
            window = it->second;
            window->Position = position;
            window->Scale = scale;
            window->BackgroundColor = color;
        } else {
            window = std::make_shared<UICanvas>();
            window->ID = id;
            window->Position = position;
            window->Scale = scale;
            window->BackgroundColor = color;
            window->ZOrder = s_Instance->m_NextZOrder++;
            s_Instance->m_CanvasStateCache[id] = window;
        }
        s_Instance->m_Canvases.push_back(window);
        s_Instance->m_CurrentActiveCanvas = window.get();
    }

    void UI::EndCanvas()
    {
        SOF_ASSERT(s_Instance, "No UI instance detected, did you forget to run Init()?");
        SOF_ASSERT(s_Instance->m_CurrentActiveCanvas,
          "No canvas currently active, make sure you run BeginCanvas before drawing to it");

        s_Instance->m_CurrentActiveCanvas = nullptr;
    }
    void UI::Text(const std::string &label, const glm::vec2 &position, const glm::vec4 &color, size_t font_index)
    {
        SOF_ASSERT(s_Instance, "No UI instance detected, did you forget to run Init()?");
        SOF_ASSERT(s_Instance->m_CurrentActiveCanvas,
          "No canvas currently active, make sure you run BeginCanvas before drawing to it");
        s_Instance->m_CurrentActiveCanvas->Elements.push_back(
          std::make_shared<UIText>(label, position, color, font_index));
    }
}// namespace SOF
