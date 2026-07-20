#include "PlaceBlock.hpp"

#include <iostream>

#include "CameraHandler.hpp"

void placeBlock(glm::ivec3 chunkLocation, glm::ivec3 blockLocation, Chunk& chunk, BlockType blockToPlace, WorldManager& worldManager, ChunkRenderer& chunkRenderer)
{
    chunkSetBlock(blockLocation.x, blockLocation.y, blockLocation.z, blockToPlace, chunk);
    if (blockHasComponent(blockToPlace)) {
        uint64_t components = blockTypeComponents[blockToPlace];
        EntityID entityID = worldManager.generateEntity(chunkLocation, blockLocation, components);
        worldManager.blockEntities[chunkLocation][blockLocation] = entityID;

        std::cout << "entityID creation = " << entityID << "\n";

        if (blockHasComponent(blockToPlace, inventoryComponentBitmask)) {
            entityManager.entities[entityID].getComponent<Inventory>().setSize(blockTypeInventorySize.at(blockToPlace));
            
            std::cout << "set size\n";
            std::cout << entityManager.entities[entityID].getComponent<Inventory>().getSize() << "\n";
        }
        std::cout << "creation chunk = " << chunkLocation.x << chunkLocation.y << chunkLocation.z << blockLocation.x << blockLocation.y << blockLocation.z << "\n";
        std::cout << "entityID after creation = " << worldManager.blockEntities.at(chunkLocation).at(blockLocation) << "\n";
    }

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

void interactWithBlock(
    glm::ivec3 chunkLocation,
    glm::ivec3 blockLocation,
    BlockType blockAtLocation,
    WorldManager& worldManager,
    ChunkRenderer& chunkRenderer,
    PlayerInventoryManager& playerInventoryManager)
{
    if (blockHasComponent(blockAtLocation, inventoryComponentBitmask)) {
        EntityID entityID = worldManager.blockEntities.at(chunkLocation).at(blockLocation);
        std::cout << "creation chunk = " << chunkLocation.x << chunkLocation.y << chunkLocation.z << blockLocation.x << blockLocation.y << blockLocation.z << "\n";
        std::cout << "entityID open = " << entityID << "\n";
        playerInventoryManager.openInventory(entityID);
    }
}

void processRightClick(
    glm::vec3 position,
    WorldManager& worldManager,
    ChunkRenderer& chunkRenderer,
    PlayerInventoryManager& playerInventoryManager)
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

    if (blockTypeIsInteractable[blockAtLocation]) {
        interactWithBlock(chunkLocation, blockLocation, BlockType::furnace, worldManager, chunkRenderer, playerInventoryManager);
    }
    if (blockAtLocation == BlockType::air) {
        placeBlock(chunkLocation, blockLocation, chunk, BlockType::furnace, worldManager, chunkRenderer);
    }
}
