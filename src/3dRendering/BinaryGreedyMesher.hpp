#pragma once

#include <glm/vec3.hpp>

#include <vector>

#include "Rendering/Vertex.hpp"
#include "World/WorldManager.hpp"

struct MeshFace
{
    uint32_t textureLayer;
    glm::vec3 location[4];
    glm::vec2 uv[4];
};

void binaryGreedyMeshChunk(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices);
void binaryGreedyMeshChunk4(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices);
