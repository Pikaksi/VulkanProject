#include <iostream>

#include "ChunkRenderer.hpp"

void ChunkRenderer::addChunksToBeRendered(glm::i32vec3 renderingChunkLocation)
{
	// dont rerender chunks rendering position did not move
	if (lastChunkRenderingLocation == renderingChunkLocation) {
		return;
	}
	lastChunkRenderingLocation = renderingChunkLocation;

	for (int x = -renderDistance; x <= renderDistance; x++) {
		for (int y = -renderDistance; y <= renderDistance; y++) {
			for (int z = -renderDistance; z <= renderDistance; z++) {

				glm::i32vec3 chunkLocation = glm::i32vec3(x, y, z) + renderingChunkLocation;

				if (!renderedChunks.contains(chunkLocation)) {
					chunksToRender.push_back(chunkLocation);
					std::cout << "added chunk to render " << x << " " << y << " " << z << "\n";
				}
			}
		}
	}
}

void ChunkRenderer::renderNewChunks(WorldManager& worldManager, VertexBufferManager& vertexBufferManager)
{
	if (chunksToRender.size() == 0) {
		return;
	}
	glm::i32vec3 chunkLocation = chunksToRender[chunksToRender.size() - 1];
	renderChunk(chunkLocation, worldManager, vertexBufferManager);
	chunksToRender.pop_back();
}

void ChunkRenderer::renderChunk(glm::i32vec3 chunkLocation, WorldManager& worldManager, VertexBufferManager& vertexBufferManager)
{
	worldManager.tryGeneratingNewChunk(chunkLocation);
	// generate adjacent chunks so that we dont have to rerender this chunk when they are generated
	chunkLocation.x -= 1;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.x += 2;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.x -= 1;

	chunkLocation.y -= 1;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.y += 2;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.y -= 1;

	chunkLocation.z -= 1;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.z += 2;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.z -= 1;

	std::cout << "rendering chunk " << chunkLocation.x << " " << chunkLocation.y << " " << chunkLocation.z << "\n";

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	generateChunkMeshData(worldManager, chunkLocation, vertices, indices);
	if (vertices.size() == 0) {
		return;
	}
	vertexBufferManager.addChunkVertices(vertices, indices);

	renderedChunks.insert(chunkLocation);
}