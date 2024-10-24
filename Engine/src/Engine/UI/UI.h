#pragma once
#include "Engine/Events/KeyEvents.h"
#include "Engine/Events/MouseEvents.h"
#include "UIElement.h"

struct FT_LibraryRec_;
typedef struct FT_LibraryRec_ *FT_Library;

namespace SOF
{
    constexpr uint32_t UILayerOffset = 1000;
    constexpr glm::vec2 UIBaseResolution = { 2560.f, 1440.f };
    constexpr uint32_t UIGlyphBaseSize = 56;

    class Texture;

    struct GlyphInfo
    {
        float Advance;
        uint32_t BearingX;
        uint32_t BearingY;
        uint32_t Width;
        uint32_t Height;
        float U0;
        float V0;
        float U1;
        float V1;
    };

    struct Font
    {
        uint32_t FontSize;
        uint32_t Ascent;
        uint32_t GlyphHeight;
        uint32_t MaxGlyphWidth;
        std::vector<GlyphInfo> GlyphInfos;
        std::shared_ptr<Texture> Atlas;
    };

    class UI
    {
        public:
        static void
          BeginCanvas(const std::string &id, const glm::vec2 &position, const glm::vec2 &scale, const glm::vec4 &color);
        static void EndCanvas();

        static void
          Text(const std::string &label, const glm::vec2 &position, const glm::vec4 &color, size_t font_index);

        static bool IsCapturingMouse();
        static bool IsCapturingInput();
        // Returns the index of the font position
        static size_t RegisterFont(const std::string &path_to_font, uint32_t font_size = 16);
        static Font *GetFont(size_t index);
        static glm::vec2 NormalizeScreenPosition(const glm::vec2 &position, const glm::vec2 &normalized_scale);
        static glm::vec2 NormalizeScreenScale(const glm::vec2 &scale);

        static glm::vec2 ScaleToScreen(const glm::vec2 &vector);


        protected:
        struct UICanvas
        {
            std::string ID;
            glm::vec2 Position = { 0.f, 0.f };
            glm::vec2 Scale = { 100.f, 100.f };
            glm::vec4 BackgroundColor = { 0.f, 0.f, 0.f, 1.f };
            int ZOrder = 0;
            bool IsHovered = false;
            uint32_t Layer = UILayerOffset;
            std::vector<std::shared_ptr<UIElement>> Elements;
        };
        static void Init();
        static void Shutdown();
        static void BeginFrame();
        static void EndFrame();
        static bool OnKeyPress(KeyPressedEvent &event);
        static bool OnKeyReleased(KeyReleasedEvent &event);
        static bool OnMouseMoved(MouseMovedEvent &event);
        static bool OnMouseButtonPress(MouseButtonPressedEvent &event);
        static bool OnMouseButtonReleased(MouseButtonReleasedEvent &event);
        static void OnWindowEvent(Event &event);

        private:
        static float ConvertPointsToPixels(float points);

        private:
        static UI *s_Instance;
        std::unordered_map<std::string, std::shared_ptr<UICanvas>> m_CanvasStateCache;
        std::vector<std::shared_ptr<UICanvas>> m_Canvases;

        UICanvas *m_FocusedCanvas = nullptr;
        UICanvas *m_CurrentActiveCanvas = nullptr;
        bool m_CapturingMouse = false;
        bool m_CapturingInput = false;
        int m_NextZOrder = 0;

        // Dunno if this is a good way to store something like this
        std::vector<Font> m_Fonts;
        FT_Library m_Library;

        friend class Game;
    };
}// namespace SOF
