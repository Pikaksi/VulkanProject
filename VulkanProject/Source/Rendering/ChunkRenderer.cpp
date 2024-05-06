#include <iostream>

#include "ChunkRenderer.hpp"

void ChunkRenderer::addChunksToBeRendered()
{

}

void ChunkRenderer::renderNewChunks(WorldManager& worldManager, VertexBufferManager& vertexBufferManager)
{
	for (glm::i32vec3 chunkLocation : chunksToRender) {
		renderChunk(chunkLocation, worldManager, vertexBufferManager);
	}
}

void ChunkRenderer::renderChunk(glm::i32vec3 chunkLocation, WorldManager& worldManager, VertexBufferManager& vertexBufferManager)
{
	worldManager.tryGeneratingNewChunk(chunkLocation);
	std::cout << "rendering chunk " << chunkLocation.x << " " << chunkLocation.y << " " << chunkLocation.z << "\n";

	std::cout << "added " << chunkLocation.x << " " << chunkLocation.y << " " << chunkLocation.z << " mesh data.\n";

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	generateChunkMeshData(worldManager, chunkLocation, vertices, indices);
	if (vertices.size() == 0) {
		return;
	}
	vertexBufferManager.addChunkVertices(vertices, indices);
}