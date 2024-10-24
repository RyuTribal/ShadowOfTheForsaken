#pragma once

namespace SOF
{
    class UIElement
    {
        public:
        ~UIElement() = default;
        virtual glm::vec2 Draw(const glm::mat4 &parent_transform,
          const glm::vec2 &parent_position,
          const glm::vec2 &parent_size,
          uint32_t parent_layer,
          const glm::vec2 &offset) = 0;
        virtual void HandleInput(const glm::vec2 &mousePos) = 0;
    };

    class UIButton : public UIElement
    {
        public:
        std::string Label;
        glm::vec2 Position;
        glm::vec2 Size;

        UIButton(const std::string &label, const glm::vec2 &position, const glm::vec2 &size)
          : Label(label), Position(position), Size(size)
        {}

        glm::vec2 Draw(const glm::mat4 &parent_transform,
          const glm::vec2 &parent_position,
          const glm::vec2 &parent_size,
          uint32_t parent_layer,
          const glm::vec2 &offset) override;

        void HandleInput(const glm::vec2 &mousePos) override;
    };

    class UIText : public UIElement
    {
        public:
        std::string TextString;
        glm::vec2 Position;
        glm::vec4 Color;
        size_t FontIndex;

        UIText(const std::string &label, const glm::vec2 &position, const glm::vec4 &color, size_t font_index)
          : TextString(label), Position(position), Color(color), FontIndex(font_index)
        {}

        glm::vec2 Draw(const glm::mat4 &parent_transform,
          const glm::vec2 &parent_position,
          const glm::vec2 &parent_size,
          uint32_t parent_layer,
          const glm::vec2 &offset) override;

        void HandleInput(const glm::vec2 &mousePos) override;
    };

}// namespace SOF