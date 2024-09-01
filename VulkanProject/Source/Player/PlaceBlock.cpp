#include "PlaceBlock.hpp"

#include <iostream>

#include "CameraHandler.hpp"

void placeBlock(glm::ivec3 chunkLocation, glm::ivec3 blockLocation, Chunk& chunk, BlockType blockToPlace, WorldManager& worldManager, ChunkRenderer& chunkRenderer)
{
    chunkSetBlock(blockLocation.x, blockLocation.y, blockLocation.z, blockToPlace, chunk);
    if (blockLocation.x == CHUNK_SIZE - 1) {
        chunkRenderer.rerenderChunk({chunkLocation.x + 1, chunkLocation.y, chunkLocation.z});
    }
    else if (blockLocation.x == 0) {
        chunkRenderer.rerenderChunk({chunkLocation.x - 1, chunkLocation.y, chunkLocation.z});
    }
    if (blockLocation.y == CHUNK_SIZE - 1) {
        chunkRenderer.rerenderChunk({chunkLocation.x, chunkLocation.y + 1, chunkLocation.z});
    }
    else if (blockLocation.y == 0) {
        chunkRenderer.rerenderChunk({chunkLocation.x, chunkLocation.y - 1, chunkLocation.z});
    }
    if (blockLocation.z == CHUNK_SIZE - 1) {
        chunkRenderer.rerenderChunk({chunkLocation.x, chunkLocation.y, chunkLocation.z + 1});
    }
    else if (blockLocation.z == 0) {
        chunkRenderer.rerenderChunk({chunkLocation.x, chunkLocation.y, chunkLocation.z - 1});
    }
    chunkRenderer.rerenderChunk(chunkLocation);
}

void processInteracting(glm::vec3 position, WorldManager& worldManager, ChunkRenderer& chunkRenderer)
{
    glm::ivec3 worldBlockLocation = floor(position);
    glm::ivec3 chunkLocation = getChunkLocation(worldBlockLocation);
    glm::ivec3 blockLocation =  {
        negativeModulo(worldBlockLocation.x, CHUNK_SIZE),
        negativeModulo(worldBlockLocation.y, CHUNK_SIZE),
        negativeModulo(worldBlockLocation.z, CHUNK_SIZE),
    };

    Chunk& chunk = worldManager.chunks.at(chunkLocation);
    BlockType blockAtLocation = chunkGetBlockAtLocation(blockLocation.x, blockLocation.y, blockLocation.z, chunk);

    if (isBlockInteractable[blockAtLocation]) {
        
    }
    else if (blockAtLocation != BlockType::air) {
        placeBlock(chunkLocation, blockLocation, chunk, BlockType::furnace, worldManager, chunkRenderer);
    }
}
