#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>

#include <vector>

#include "Chunk.hpp"
#include "BlockType.hpp"
#include "Rendering/Vertex.hpp"

struct MeshFace
{
    uint32_t textureLayer;
    glm::vec3 location[4];
    glm::vec2 uv[4];
};

void blockArrayMesher(std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)> blocks,
                      std::vector<MeshFace>& faces);
