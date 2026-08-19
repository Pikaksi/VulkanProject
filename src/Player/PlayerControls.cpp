#include "PlayerControls.hpp"

#include <iostream>

#include "PlaceBlock.hpp"
#include "PlayerInputHandler.hpp"
#include "blockEntityManager.hpp"

void updatePlayerControls(glm::vec3 position,
                          WorldManager& worldManager,
                          BlockEntityManager& blockEntityManager,
                          ChunkRenderer& chunkRenderer,
                          PlayerInfo& playerInfo)
{
    if (PlayerInputHandler::getInstance().mouseRightPressed) {
        glm::ivec3 worldBlockLocation = floor(position);
        glm::ivec3 chunkLocation = getChunkLocation(worldBlockLocation);
        glm::ivec3 blockLocation = {
            alwaysPosModulo(worldBlockLocation.x, CHUNK_SIZE),
            alwaysPosModulo(worldBlockLocation.y, CHUNK_SIZE),
            alwaysPosModulo(worldBlockLocation.z, CHUNK_SIZE),
        };

        Chunk& chunk = worldManager.chunks.at(chunkLocation);
        BlockType block = chunkGetBlockAtLocation(blockLocation.x, blockLocation.y, blockLocation.z, chunk);

        if (block == BlockType::air) {
            playerInfo.isLookingAtBlock = false;
            placeBlock(
                chunkLocation, blockLocation, BlockType::furnace, worldManager, blockEntityManager, chunkRenderer);
        }
        else {
            playerInfo.isLookingAtBlock = true;
            playerInfo.lookingAtLocation = blockLocation;
        }
    }
}
