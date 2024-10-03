#pragma once


namespace SOF
{
    class DebugWindow
    {
        public:
        DebugWindow();
        void Render(Scene *current_scene);

        bool IsWindowActive() { return m_WindowActive; }

        private:
        float m_BgColor[3] = { 0.f, 0.f, 0.f };
        char m_AssetFilePath[256];
        char m_AssetHandle[256];
        char m_DeletedAssetHandle[256];
        bool m_WindowActive = false;
    };
}// namespace SOF