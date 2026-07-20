#pragma once

#include <vector>

#include "Rendering/Vertex.hpp"
#include "World/Chunk.hpp"
#include "World/BlockDataLookup.hpp"
#include "World/WorldManager.hpp"

// old version for benchmarking

void generateChunkMeshData(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices);
void addBlockMeshData(int x, int y, int z, int locationX, int locationY, int locationZ, std::vector<Vertex>& vertices, BlockType blockType,
	Chunk* chunk, Chunk* chunkPX, Chunk* chunkNX, Chunk* chunkPY, Chunk* chunkNY, Chunk* chunkPZ, Chunk* chunkNZ);

void addQuadToFourLastVertices(uint32_t verticesSize, std::vector<uint32_t>& indices);
void addQuad(int index1, int index2, int index3, int index4, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
