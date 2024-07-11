#include "VertexCreator.hpp"

#include <iostream>
#include <vector>
#include <array>

#include "glm/vec3.hpp"

#include "BlockTexCoordinateLookup.hpp"

void generateChunkMeshData(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices)
{
	Chunk* chunk = &worldManager.chunks[chunkLocation];
	Chunk* chunkPX = nullptr;
	Chunk* chunkNX = nullptr;
	Chunk* chunkPY = nullptr;
	Chunk* chunkNY = nullptr;
	Chunk* chunkPZ = nullptr;
	Chunk* chunkNZ = nullptr;

	glm::i32vec3 pX = glm::i32vec3(chunkLocation.x + 1, chunkLocation.y, chunkLocation.z);
	glm::i32vec3 nX = glm::i32vec3(chunkLocation.x - 1, chunkLocation.y, chunkLocation.z);
	glm::i32vec3 pY = glm::i32vec3(chunkLocation.x, chunkLocation.y + 1, chunkLocation.z);
	glm::i32vec3 nY = glm::i32vec3(chunkLocation.x, chunkLocation.y - 1, chunkLocation.z);
	glm::i32vec3 pZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z + 1);
	glm::i32vec3 nZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z - 1);

	if (worldManager.chunks.contains(pX)) {
		chunkPX = &worldManager.chunks[pX];
	}
	if (worldManager.chunks.contains(nX)) {
		chunkNX = &worldManager.chunks[nX];
	}
	if (worldManager.chunks.contains(pY)) {
		chunkPY = &worldManager.chunks[pY];
	}
	if (worldManager.chunks.contains(nY)) {
		chunkNY = &worldManager.chunks[nY];
	}
	if (worldManager.chunks.contains(pZ)) {
		chunkPZ = &worldManager.chunks[pZ];
	}
	if (worldManager.chunks.contains(nZ)) {
		chunkNZ = &worldManager.chunks[nZ];
	}


	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				BlockType blockType = chunkGetBlockAtLocation(x, y, z, chunk);
				if (blockType == air) {
					continue;
				}

				addBlockMeshData(x, y, z, x + chunkLocation.x * CHUNK_SIZE, y + chunkLocation.y * CHUNK_SIZE, z + chunkLocation.z * CHUNK_SIZE, vertices, blockType
					, chunk, chunkPX, chunkNX, chunkPY, chunkNY, chunkPZ, chunkNZ);
			}
		}
	}
}

// chunkPX means chunk in Positive X direction
void addBlockMeshData(int x, int y, int z, int locationX, int locationY, int locationZ, std::vector<Vertex>& vertices, BlockType blockType,
	Chunk* chunk, Chunk* chunkPX, Chunk* chunkNX, Chunk* chunkPY, Chunk* chunkNY, Chunk* chunkPZ, Chunk* chunkNZ)
{
	std::array<float, 6> textureArrayIndices = blockTypeToTexLayer.at(blockType);
	// right
	if ((x != CHUNK_SIZE - 1 && !isSolidBlock[chunkGetBlockAtLocation(x + 1, y, z, chunk)])
		|| (x == CHUNK_SIZE - 1 && chunkPX != nullptr && !isSolidBlock[chunkGetBlockAtLocation(0, y, z, chunkPX)])) {

		vertices.push_back(Vertex{ {locationX + 1, locationY,     locationZ    }, {0.9f, 0.9f, 0.9f}, {0.0f, 1.0f}, textureArrayIndices[0] });
		vertices.push_back(Vertex{ {locationX + 1, locationY,     locationZ + 1}, {0.9f, 0.9f, 0.9f}, {1.0f, 1.0f}, textureArrayIndices[0] });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ + 1}, {0.9f, 0.9f, 0.9f}, {1.0f, 0.0f}, textureArrayIndices[0] });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ    }, {0.9f, 0.9f, 0.9f}, {0.0f, 0.0f}, textureArrayIndices[0] });
	}

	// left
	if ((x != 0 && !isSolidBlock[chunkGetBlockAtLocation(x - 1, y, z, chunk)])
		|| (x == 0 && chunkNX != nullptr && !isSolidBlock[chunkGetBlockAtLocation(CHUNK_SIZE - 1, y, z, chunkNX)])) {

		vertices.push_back(Vertex{ {locationX,     locationY,     locationZ + 1}, {0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, textureArrayIndices[1] });
		vertices.push_back(Vertex{ {locationX,     locationY,     locationZ    }, {0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, textureArrayIndices[1] });
		vertices.push_back(Vertex{ {locationX,     locationY + 1, locationZ    }, {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, textureArrayIndices[1] });
		vertices.push_back(Vertex{ {locationX,     locationY + 1, locationZ + 1}, {0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, textureArrayIndices[1] });
	}

	// up
	if ((y != CHUNK_SIZE - 1 && !isSolidBlock[chunkGetBlockAtLocation(x, y + 1, z, chunk)])
		|| (y == CHUNK_SIZE - 1 && chunkPY != nullptr && !isSolidBlock[chunkGetBlockAtLocation(x, 0, z, chunkPY)])) {

		vertices.push_back(Vertex{ {locationX    , locationY + 1, locationZ    }, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, textureArrayIndices[2] });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ    }, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, textureArrayIndices[2] });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ + 1}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, textureArrayIndices[2] });
		vertices.push_back(Vertex{ {locationX    , locationY + 1, locationZ + 1}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, textureArrayIndices[2] });
	}

	// down
	if ((y != 0 && !isSolidBlock[chunkGetBlockAtLocation(x, y - 1, z, chunk)])
		|| (y == 0 && chunkNY != nullptr && !isSolidBlock[chunkGetBlockAtLocation(x, CHUNK_SIZE - 1, z, chunkNY)])) {

		vertices.push_back(Vertex{ {locationX    , locationY,     locationZ + 1}, {0.4f, 0.4f, 0.4f}, {0.0f, 1.0f}, textureArrayIndices[3] });
		vertices.push_back(Vertex{ {locationX + 1, locationY,     locationZ + 1}, {0.4f, 0.4f, 0.4f}, {1.0f, 1.0f}, textureArrayIndices[3] });
		vertices.push_back(Vertex{ {locationX + 1, locationY,     locationZ    }, {0.4f, 0.4f, 0.4f}, {1.0f, 0.0f}, textureArrayIndices[3] });
		vertices.push_back(Vertex{ {locationX    , locationY,     locationZ    }, {0.4f, 0.4f, 0.4f}, {0.0f, 0.0f}, textureArrayIndices[3] });
	}

	// forward
	if ((z != CHUNK_SIZE - 1 && !isSolidBlock[chunkGetBlockAtLocation(x, y, z + 1, chunk)])
		|| (z == CHUNK_SIZE - 1 && chunkPZ != nullptr && !isSolidBlock[chunkGetBlockAtLocation(x, y, 0, chunkPZ)])) {

		vertices.push_back(Vertex{ {locationX + 1, locationY    , locationZ + 1}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}, textureArrayIndices[4] });
		vertices.push_back(Vertex{ {locationX    , locationY    , locationZ + 1}, {0.8f, 0.8f, 0.8f}, {1.0f, 1.0f}, textureArrayIndices[4] });
		vertices.push_back(Vertex{ {locationX    , locationY + 1, locationZ + 1}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f}, textureArrayIndices[4] });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ + 1}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f}, textureArrayIndices[4] });
	}

	// backward
	if ((z != 0 && !isSolidBlock[chunkGetBlockAtLocation(x, y, z - 1, chunk)])
		|| (z == 0 && chunkNZ != nullptr && !isSolidBlock[chunkGetBlockAtLocation(x, y, CHUNK_SIZE - 1, chunkNZ)])) {

		vertices.push_back(Vertex{ {locationX    , locationY,     locationZ    }, {0.6f, 0.6f, 0.6f}, {0.0f, 1.0f}, textureArrayIndices[5] });
		vertices.push_back(Vertex{ {locationX + 1, locationY,     locationZ    }, {0.6f, 0.6f, 0.6f}, {1.0f, 1.0f}, textureArrayIndices[5] });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ    }, {0.6f, 0.6f, 0.6f}, {1.0f, 0.0f}, textureArrayIndices[5] });
		vertices.push_back(Vertex{ {locationX    , locationY + 1, locationZ    }, {0.6f, 0.6f, 0.6f}, {0.0f, 0.0f}, textureArrayIndices[5] });
	}
}

void addQuadToFourLastVertices(uint32_t verticesSize, std::vector<uint32_t>& indices)
{
	indices.push_back(verticesSize - 4);
	indices.push_back(verticesSize - 3);
	indices.push_back(verticesSize - 2);
	indices.push_back(verticesSize - 2);
	indices.push_back(verticesSize - 3);
	indices.push_back(verticesSize - 1);
}

void addQuad(int index1, int index2, int index3, int index4, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	indices.push_back(index1);
	indices.push_back(index2);
	indices.push_back(index3);
	indices.push_back(index3);
	indices.push_back(index4);
	indices.push_back(index1);
}