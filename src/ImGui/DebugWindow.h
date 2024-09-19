#pragma once


namespace SOF
{
    class DebugWindow
    {
        public:
        DebugWindow();
        void Render();

        private:
        float m_BgColor[3] = { 0.f, 0.f, 0.f };
        char m_AssetFilePath[256];
        char m_AssetHandle[256];
        char m_DeletedAssetHandle[256];
    };
}// namespace SOF