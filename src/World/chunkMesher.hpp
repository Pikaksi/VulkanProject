#pragma once
#include "World/WorldManager.hpp"
#include "Vertex.hpp"

void createChunkMeshFullDetail(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices);
void createChunkMeshLod(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<VertexLod>& vertices, int lod);
