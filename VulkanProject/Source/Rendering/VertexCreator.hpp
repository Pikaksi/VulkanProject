#pragma once

#include "Rendering/Vertex.hpp"
#include "World/Chunk.hpp"
#include "World/BlockDataLookup.hpp"
#include "World/WorldManager.hpp"

#include <vector>

void generateChunkMeshData(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
void addBlockMeshData(int x, int y, int z, int vertexX, int vertexY, int vertexZ, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices
	, short* chunk, short* chunkPX, short* chunkNX, short* chunkPY, short* chunkNY, short* chunkPZ, short* chunkNZ);

void addQuadToFourLastVertices(uint32_t verticesSize, std::vector<uint32_t>& indices);
void addQuad(int index1, int index2, int index3, int index4, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
