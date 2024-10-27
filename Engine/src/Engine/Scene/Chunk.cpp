#include "pch.h"
#include "Chunk.h"
#include "Entity.h"
#include "Components.h"
#include "Scene.h"

namespace SOF
{
    ChunkManager::ChunkManager(float chunk_size, Scene *context) : m_ChunkSize(chunk_size), m_Context(context) {}

    void ChunkManager::AddEntity(UUID entity_id)
    {
        glm::vec3 position = m_Context->GetEntity(entity_id)->GetComponent<TransformComponent>()->Translation;
        glm::ivec2 chunkCoords = GetChunkCoords({ position.x, position.y });

        auto &chunk = m_Chunks[chunkCoords];
        chunk.Position = glm::vec2(chunkCoords) * m_ChunkSize;
        chunk.Entities.push_back(entity_id);

        m_EntityToChunk[entity_id] = chunkCoords;
    }

    void ChunkManager::RemoveEntity(UUID entity_id)
    {
        auto it = m_EntityToChunk.find(entity_id);
        if (it != m_EntityToChunk.end()) {
            glm::ivec2 chunkCoords = it->second;
            auto chunkIt = m_Chunks.find(chunkCoords);
            if (chunkIt != m_Chunks.end()) {
                auto &entities = chunkIt->second.Entities;
                entities.erase(std::remove(entities.begin(), entities.end(), entity_id), entities.end());
                if (entities.empty()) { m_Chunks.erase(chunkIt); }
            }
            m_EntityToChunk.erase(it);
        }
    }

    void ChunkManager::UpdateEntity(UUID entity_id)
    {
        if (!m_Context->GetEntity(entity_id)->HasComponent<SpriteComponent>()) { return; }
        glm::vec3 position = m_Context->GetEntity(entity_id)->GetComponent<TransformComponent>()->Translation;
        glm::ivec2 newChunkCoords = GetChunkCoords({ position.x, position.y });

        auto it = m_EntityToChunk.find(entity_id);
        if (it != m_EntityToChunk.end()) {
            glm::ivec2 currentChunkCoords = it->second;
            if (currentChunkCoords != newChunkCoords) {
                RemoveEntity(entity_id);
                AddEntity(entity_id);
            }
        } else {
            AddEntity(entity_id);
        }
    }

    std::vector<Chunk *>
      ChunkManager::GetVisibleChunks(const glm::vec2 &camera_position, float camera_width, float camera_height)
    {
        float margin = m_ChunkSize * 0.1f;
        glm::ivec2 minCoords =
          GetChunkCoords(camera_position - glm::vec2(camera_width / 2 + margin, camera_height / 2 + margin));
        glm::ivec2 maxCoords =
          GetChunkCoords(camera_position + glm::vec2(camera_width / 2 + margin, camera_height / 2 + margin));

        std::vector<Chunk *> visibleChunks;

        for (int x = minCoords.x; x <= maxCoords.x; ++x) {
            for (int y = minCoords.y; y <= maxCoords.y; ++y) {
                glm::ivec2 chunkCoords = { x, y };
                if (m_Chunks.find(chunkCoords) != m_Chunks.end()) { visibleChunks.push_back(&m_Chunks[chunkCoords]); }
            }
        }

        return visibleChunks;
    }

    glm::ivec2 ChunkManager::GetChunkCoords(const glm::vec2 &position)
    {
        float adjustedX = position.x / m_ChunkSize;
        float adjustedY = position.y / m_ChunkSize;

        return glm::ivec2(static_cast<int>(round(adjustedX)), static_cast<int>(round(adjustedY)));
    }
}// namespace SOF