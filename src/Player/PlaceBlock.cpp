#include "PlaceBlock.hpp"

#include <iostream>

#include "BlockDataLookup.hpp"
#include "assertm.hpp"
#include "blockEntity.hpp"

void placeBlock(glm::ivec3 chunkLocation,
                glm::ivec3 blockLocation,
                BlockType blockToPlace,
                WorldManager& worldManager,
                BlockEntityManager& blockEntityManager,
                ChunkRenderer& chunkRenderer)
{
    assertm(worldManager.chunks.contains(chunkLocation), "Trying to place to a chunk that is not rendered");

    Chunk& chunk = worldManager.chunks.at(chunkLocation);
    chunkSetBlock(blockLocation.x, blockLocation.y, blockLocation.z, blockToPlace, chunk);

    BlockEntityType blockEntityType = blockTypeToBlockEntityType[blockToPlace];
    if (blockEntityType != BlockEntityType::none) {
        glm::i32vec3 worldLocation = blockLocation + chunkLocation * CHUNK_SIZE;
        blockEntityManager.createEntity(worldLocation, blockEntityType);
    }

    if (blockLocation.x == CHUNK_SIZE - 1) {
        chunkRenderer.rerenderChunkAgain({chunkLocation.x + 1, chunkLocation.y, chunkLocation.z});
    }
    else if (blockLocation.x == 0) {
        chunkRenderer.rerenderChunkAgain({chunkLocation.x - 1, chunkLocation.y, chunkLocation.z});
    }
    if (blockLocation.y == CHUNK_SIZE - 1) {
        chunkRenderer.rerenderChunkAgain({chunkLocation.x, chunkLocation.y + 1, chunkLocation.z});
    }
    else if (blockLocation.y == 0) {
        chunkRenderer.rerenderChunkAgain({chunkLocation.x, chunkLocation.y - 1, chunkLocation.z});
    }
    if (blockLocation.z == CHUNK_SIZE - 1) {
        chunkRenderer.rerenderChunkAgain({chunkLocation.x, chunkLocation.y, chunkLocation.z + 1});
    }
    else if (blockLocation.z == 0) {
        chunkRenderer.rerenderChunkAgain({chunkLocation.x, chunkLocation.y, chunkLocation.z - 1});
    }
    chunkRenderer.rerenderChunkAgain(chunkLocation);
}
