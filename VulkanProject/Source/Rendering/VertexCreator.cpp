#include <iostream>
#include <vector>

#include "glm/vec3.hpp"

#include "VertexCreator.hpp"

void generateChunkMeshData(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	short* chunk = worldManager.chunks[chunkLocation].blocks;
	short* chunkPX = nullptr;
	short* chunkNX = nullptr;
	short* chunkPY = nullptr;
	short* chunkNY = nullptr;
	short* chunkPZ = nullptr;
	short* chunkNZ = nullptr;

	glm::i32vec3 pX = glm::i32vec3(chunkLocation.x + 1, chunkLocation.y, chunkLocation.z);
	glm::i32vec3 nX = glm::i32vec3(chunkLocation.x - 1, chunkLocation.y, chunkLocation.z);
	glm::i32vec3 pY = glm::i32vec3(chunkLocation.x, chunkLocation.y + 1, chunkLocation.z);
	glm::i32vec3 nY = glm::i32vec3(chunkLocation.x, chunkLocation.y - 1, chunkLocation.z);
	glm::i32vec3 pZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z + 1);
	glm::i32vec3 nZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z - 1);

	if (worldManager.chunks.contains(pX)) {
		chunkPX = worldManager.chunks[pX].blocks;
	}
	if (worldManager.chunks.contains(nX)) {
		chunkNX = worldManager.chunks[nX].blocks;
	}
	if (worldManager.chunks.contains(pY)) {
		chunkPY = worldManager.chunks[pY].blocks;
	}
	if (worldManager.chunks.contains(nY)) {
		chunkNY = worldManager.chunks[nY].blocks;
	}
	if (worldManager.chunks.contains(pZ)) {
		chunkPZ = worldManager.chunks[pZ].blocks;
	}
	if (worldManager.chunks.contains(nZ)) {
		chunkNZ = worldManager.chunks[nZ].blocks;
	}


	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				short blockType = chunk[chunkLocationToIndex(x, y, z)];
				if (blockType == air) {
					continue;
				}

				addBlockMeshData(x, y, z, x + chunkLocation.x * CHUNK_SIZE, y + chunkLocation.y * CHUNK_SIZE, z + chunkLocation.z * CHUNK_SIZE, vertices, indices
					, chunk, chunkPX, chunkNX, chunkPY, chunkNY, chunkPZ, chunkNZ);
			}
		}
	}
}

void addBlockMeshData(int x, int y, int z, int locationX, int locationY, int locationZ, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
	short* chunk, short* chunkPX, short* chunkNX, short* chunkPY, short* chunkNY, short* chunkPZ, short* chunkNZ) // chunkPX means chunk in Positive X direction
{
	// right
	if ((x != CHUNK_SIZE - 1 && !isSolidBlock[chunk[chunkLocationToIndex(x + 1, y, z)]])
		|| (x == CHUNK_SIZE - 1 && chunkPX != nullptr && !isSolidBlock[chunkPX[chunkLocationToIndex(0, y, z)]])) {

		vertices.push_back(Vertex{ {locationX + 1, locationY,     locationZ    }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX + 1, locationY,     locationZ + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ    }, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
		addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);
	}

	// left
	if ((x != 0 && !isSolidBlock[chunk[chunkLocationToIndex(x - 1, y, z)]])
		|| (x == 0 && chunkNX != nullptr && !isSolidBlock[chunkNX[chunkLocationToIndex(CHUNK_SIZE - 1, y, z)]])) {

		vertices.push_back(Vertex{ {locationX,     locationY,     locationZ    }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX,     locationY + 1, locationZ    }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX,     locationY,     locationZ + 1}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
		vertices.push_back(Vertex{ {locationX,     locationY + 1, locationZ + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
		addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);
	}

	// up
	if ((y != CHUNK_SIZE - 1 && !isSolidBlock[chunk[chunkLocationToIndex(x, y + 1, z)]])
		|| (y == CHUNK_SIZE - 1 && chunkPY != nullptr && !isSolidBlock[chunkPY[chunkLocationToIndex(x, 0, z)]])) {

		vertices.push_back(Vertex{ {locationX    , locationY + 1, locationZ    }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ    }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX    , locationY + 1, locationZ + 1}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
		addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);
	}

	// down
	if ((y != 0 && !isSolidBlock[chunk[chunkLocationToIndex(x, y - 1, z)]])
		|| (y == 0 && chunkNY != nullptr && !isSolidBlock[chunkNY[chunkLocationToIndex(x, CHUNK_SIZE - 1, z)]])) {

		vertices.push_back(Vertex{ {locationX    , locationY,     locationZ    }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX    , locationY,     locationZ + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX + 1, locationY,     locationZ    }, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
		vertices.push_back(Vertex{ {locationX + 1, locationY,     locationZ + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
		addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);
	}

	// forward
	if ((z != CHUNK_SIZE - 1 && !isSolidBlock[chunk[chunkLocationToIndex(x, y, z + 1)]])
		|| (z == CHUNK_SIZE - 1 && chunkPZ != nullptr && !isSolidBlock[chunkPZ[chunkLocationToIndex(x, y, 0)]])) {

		vertices.push_back(Vertex{ {locationX    , locationY    , locationZ + 1}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX    , locationY + 1, locationZ + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX + 1, locationY    , locationZ + 1}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
		addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);
	}

	// backward
	std::cout << "x = " << x << " y = " << y << " z = " << z << " ";
	if (z != 0) {
		std::cout << " in chunk block = " << chunk[chunkLocationToIndex(x, y, z - 1)]  << " is not solid = " << !isSolidBlock[chunk[chunkLocationToIndex(x, y, z - 1)]] << " ";
	}
	else {
		if (chunkNZ != nullptr) {
			std::cout << " not NULL pointer = " << !isSolidBlock[chunkNZ[chunkLocationToIndex(x, y, CHUNK_SIZE - 1)]] << " ";
		}
	}
	std::cout << "\n";
	if ((z != 0 && !isSolidBlock[chunk[chunkLocationToIndex(x, y, z - 1)]])
		|| (z == 0 && chunkNZ != nullptr && !isSolidBlock[chunkNZ[chunkLocationToIndex(x, y, CHUNK_SIZE - 1)]])) {

		std::cout << "made face\n";
		vertices.push_back(Vertex{ {locationX    , locationY,     locationZ    }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX + 1, locationY,     locationZ    }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
		vertices.push_back(Vertex{ {locationX    , locationY + 1, locationZ    }, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
		vertices.push_back(Vertex{ {locationX + 1, locationY + 1, locationZ    }, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
		addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);
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