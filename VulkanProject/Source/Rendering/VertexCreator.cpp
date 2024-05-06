#include <iostream>
#include <vector>

#include "glm/vec3.hpp"

#include "VertexCreator.hpp"

void generateChunkMeshData(Chunk chunk, glm::vec3 chunkLocation, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				__int16 blockType = chunk.blocks[chunkLocationToIndex(x, y, z)];
				if (blockType == air) {
					continue;
				}

				addBlockMeshData(x + chunkLocation.x * CHUNK_SIZE, y + chunkLocation.y * CHUNK_SIZE, z + chunkLocation.z * CHUNK_SIZE, vertices, indices);
			}
		}
	}
}

void addBlockMeshData(int x, int y, int z, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	// right left
	vertices.push_back(Vertex{ {x + 1, y,     z    }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
	vertices.push_back(Vertex{ {x + 1, y,     z + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
	vertices.push_back(Vertex{ {x + 1, y + 1, z    }, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
	vertices.push_back(Vertex{ {x + 1, y + 1, z + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
	addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);

	vertices.push_back(Vertex{ {x,     y,     z    }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
	vertices.push_back(Vertex{ {x,     y + 1, z    }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
	vertices.push_back(Vertex{ {x,     y,     z + 1}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
	vertices.push_back(Vertex{ {x,     y + 1, z + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
	addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);

	// up down
	vertices.push_back(Vertex{ {x    , y + 1, z    }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
	vertices.push_back(Vertex{ {x + 1, y + 1, z    }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
	vertices.push_back(Vertex{ {x    , y + 1, z + 1}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
	vertices.push_back(Vertex{ {x + 1, y + 1, z + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
	addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);

	vertices.push_back(Vertex{ {x    , y,     z    }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
	vertices.push_back(Vertex{ {x    , y,     z + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
	vertices.push_back(Vertex{ {x + 1, y,     z    }, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
	vertices.push_back(Vertex{ {x + 1, y,     z + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
	addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);

	// forward backward
	vertices.push_back(Vertex{ {x    , y    , z + 1}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
	vertices.push_back(Vertex{ {x    , y + 1, z + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
	vertices.push_back(Vertex{ {x + 1, y    , z + 1}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
	vertices.push_back(Vertex{ {x + 1, y + 1, z + 1}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
	addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);

	vertices.push_back(Vertex{ {x    , y,     z    }, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });
	vertices.push_back(Vertex{ {x + 1, y,     z    }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} });
	vertices.push_back(Vertex{ {x    , y + 1, z    }, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} });
	vertices.push_back(Vertex{ {x + 1, y + 1, z    }, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} });
	addQuadToFourLastVertices(static_cast<uint32_t>(vertices.size()), indices);
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