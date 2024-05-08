#include <iostream>

#include "ChunkRenderer.hpp"

void ChunkRenderer::addChunksToBeRendered(glm::i32vec3 renderingChunkLocation)
{
	// dont rerender chunks rendering position did not move
	if (lastChunkRenderingLocation == renderingChunkLocation) {
		return;
	}
	int minDistanceToNewChunk = std::max(
		abs(lastChunkRenderingLocation.x - renderingChunkLocation.x),
		std::max(
		abs(lastChunkRenderingLocation.y - renderingChunkLocation.y),
		abs(lastChunkRenderingLocation.z - renderingChunkLocation.z))
	);
	int startingDistance = std::max(0, renderDistance - minDistanceToNewChunk);

	lastChunkRenderingLocation = renderingChunkLocation;

	int cubesChecked = 0;
	std::cout << "min distance for new chunk = " << startingDistance << "\n";

	// Render close by chunks first.
	// Checks a hollow box for each distance starting from the smallest.
	for (int distance = startingDistance; distance < renderDistance; distance++) {
		// x direction with all edges
		for (int z = -distance; z <= distance; z++) {
			for (int y = -distance; y <= distance; y++) {

				cubesChecked += 2;
				tryAddChunksToRender(glm::i32vec3(distance, y, z) + renderingChunkLocation);
				tryAddChunksToRender(glm::i32vec3(-distance, y, z) + renderingChunkLocation);
			}
		}

		// z direction with not x axis edges
		for (int x = -distance + 1; x <= distance - 1; x++) {
			for (int y = -distance; y <= distance; y++) {

				cubesChecked += 2;
				tryAddChunksToRender(glm::i32vec3(x, y, distance) + renderingChunkLocation);
				tryAddChunksToRender(glm::i32vec3(x, y, -distance) + renderingChunkLocation);
			}
		}

		// y direction with no edges
		for (int x = -distance + 1; x <= distance - 1; x++) {
			for (int z = -distance + 1; z <= distance - 1; z++) {

				cubesChecked += 2;
				tryAddChunksToRender(glm::i32vec3(x, distance, z) + renderingChunkLocation);
				tryAddChunksToRender(glm::i32vec3(x, -distance, z) + renderingChunkLocation);
			}
		}
	}
	std::cout << "checked " << cubesChecked << " chunks to be rendered\n";
}

void ChunkRenderer::tryAddChunksToRender(glm::i32vec3 chunkLocation)
{
	if (!renderedChunks.contains(chunkLocation)) {
		chunksToRender.push(chunkLocation);
		//std::cout << "added chunk to render " << chunkLocation.x << " " << chunkLocation.y << " " << chunkLocation.z << "\n";
	}
}

void ChunkRenderer::renderNewChunks(WorldManager& worldManager, VertexBufferManager& vertexBufferManager)
{
	if (chunksToRender.size() == 0) {
		return;
	}
	glm::i32vec3 chunkLocation = chunksToRender.front();
	renderChunk(chunkLocation, worldManager, vertexBufferManager);
	chunksToRender.pop();
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

	//std::cout << "rendering chunk " << chunkLocation.x << " " << chunkLocation.y << " " << chunkLocation.z << "\n";

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	generateChunkMeshData(worldManager, chunkLocation, vertices, indices);
	if (vertices.size() == 0) {
		return;
	}
	vertexBufferManager.addChunkVertices(vertices, indices);

	renderedChunks.insert(chunkLocation);
}