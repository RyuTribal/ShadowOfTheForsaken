#pragma once

namespace std
{
    template<> struct hash<glm::ivec2>
    {
        std::size_t operator()(const glm::ivec2 &v) const noexcept
        {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };
}// namespace std

namespace SOF
{
    class Scene;
    struct Chunk
    {
        std::vector<UUID> Entities;
        glm::vec2 Position;
    };

    class ChunkManager
    {
        public:
        ChunkManager(float chunk_size, Scene *context);
        void AddEntity(UUID entity_id);
        void RemoveEntity(UUID entity_id);
        void UpdateEntity(UUID entity_id);
        std::vector<Chunk *>
          GetVisibleChunks(const glm::vec2 &camera_position, float camera_width, float camera_height);

        private:
        glm::ivec2 GetChunkCoords(const glm::vec2 &position);

        private:
        float m_ChunkSize;
        std::unordered_map<glm::ivec2, Chunk> m_Chunks;
        std::unordered_map<UUID, glm::ivec2> m_EntityToChunk;
        Scene *m_Context;
    };

}// namespace SOF