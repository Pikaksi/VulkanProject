#pragma once
#include "World/WorldManager.hpp"
#include "Vertex.hpp"

void createChunkMesh(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices);
void createChunkMeshLod(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices, int lod);
