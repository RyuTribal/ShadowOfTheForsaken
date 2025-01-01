#include "pch.h"
#include "UIElement.h"
#include "Engine/Renderer/Texture.h"
#include "UI.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/Game.h"
#include <glm/ext/matrix_transform.hpp>

namespace SOF
{
    glm::vec2 UIButton::Draw(const glm::mat4 &parent_transform,
      const glm::vec2 &parent_position,
      const glm::vec2 &parent_size,
      uint32_t parent_layer,
      const glm::vec2 &offset)
    {
        // glm::vec2 absolutePosition = parent_position + Position;
        /*s_Instance->m_Renderer->DrawQuad(absolutePosition, Size, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        s_Instance->m_Renderer->DrawText(Label, absolutePosition + glm::vec2(10, 10));*/

        return { 0.f, 0.f };
    }
    void UIButton::HandleInput(const glm::vec2 &mousePos) {}

    glm::vec2 UIText::Draw(const glm::mat4 &parent_transform,
      const glm::vec2 &parent_position,
      const glm::vec2 &parent_size,
      uint32_t parent_layer,
      const glm::vec2 &offset)
    {
        Font *font = UI::GetFont(FontIndex);

        uint32_t max_lines = static_cast<uint32_t>(parent_size.y / font->GlyphHeight);
        std::vector<std::string> lines;
        std::string current_line = "";
        uint32_t longest_line_length = 0;

        float current_line_width = 0.0f;
        uint32_t max_chars_per_line = 0;

        for (char c : TextString) {
            uint32_t ascii_value = static_cast<uint32_t>(c);
            if (ascii_value < 32 || ascii_value > 126) { ascii_value = 32; }
            GlyphInfo &glyph = font->GlyphInfos[ascii_value - 32];
            current_line_width += glyph.Advance;
            if (current_line_width > parent_size.x || c == '\n') {
                lines.push_back(current_line);
                longest_line_length = std::max(longest_line_length, static_cast<uint32_t>(current_line.size()));
                current_line.clear();
                current_line_width = glyph.Advance;
                if (lines.size() >= max_lines) { break; }
            }
            if (c != '\n') { current_line += c; }
        }
        if (!current_line.empty() && lines.size() < max_lines) {
            lines.push_back(current_line);
            longest_line_length = std::max(longest_line_length, static_cast<uint32_t>(current_line.size()));
        }

        float x_cursor = 0.0f;
        float y_cursor = font->Ascent;

        for (int i = 0; i < lines.size(); i++) {
            x_cursor = 0.0f;
            for (char c : lines[i]) {
                uint32_t ascii_value = static_cast<uint32_t>(c);
                if (ascii_value < 32 || ascii_value > 126) { ascii_value = 32; }
                GlyphInfo &glyph = font->GlyphInfos[ascii_value - 32];
                glm::vec2 glyph_position = glm::vec2(x_cursor, y_cursor);
                glm::vec2 glyph_size = glm::vec2(glyph.Width, font->GlyphHeight);

                SpriteData glyph_sprite;
                glyph_sprite.TextureRef = font->Atlas.get();
                glyph_sprite.ShaderHandle = "font";
                glyph_sprite.Color = Color;
                glyph_sprite.Layer = parent_layer + 1;
                glm::vec2 absolute_pos = parent_position + offset + Position + glyph_position;

                glm::vec2 normalized_size = UI::NormalizeScreenScale(glyph_size);
                glm::vec2 normalized_pos = UI::NormalizeScreenPosition(absolute_pos, normalized_size);
                glyph_sprite.Transform = glm::translate(glm::mat4(1.0f), glm::vec3(normalized_pos, 0.0f));
                glyph_sprite.Transform = glm::scale(glyph_sprite.Transform, glm::vec3(normalized_size, 1.0f));

                glyph_sprite.TileUV = { glm::vec2(glyph.U0, glyph.V1),
                    glm::vec2(glyph.U1, glyph.V1),
                    glm::vec2(glyph.U0, glyph.V0),
                    glm::vec2(glyph.U1, glyph.V0) };
                Renderer::SubmitSquare(&glyph_sprite);
                x_cursor += glyph.Advance;
            }
            y_cursor += font->GlyphHeight;
        }

        return { x_cursor, y_cursor };
    }

    void UIText::HandleInput(const glm::vec2 &mousePos) { return; }
}// namespace SOF
