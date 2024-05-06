#pragma once

#include "Rendering/Vertex.hpp"
#include "World/Chunk.hpp"

#include <vector>

void generateChunkMeshData(Chunk chunk, glm::vec3 chunkLocation, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
void addBlockMeshData(int x, int y, int z, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

void addQuadToFourLastVertices(uint32_t verticesSize, std::vector<uint32_t>& indices);
void addQuad(int index1, int index2, int index3, int index4, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
