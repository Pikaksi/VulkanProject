#include "PlayerControls.hpp"

#include <iostream>

#include "PlaceBlock.hpp"
#include "PlayerInputHandler.hpp"

void updatePlayerControls(
    glm::vec3 position,
    WorldManager& worldManager,
    ChunkRenderer& chunkRenderer,
    PlayerInventoryManager& playerInventoryManager)
{
    if (PlayerInputHandler::getInstance().mouseRightPressed) {
        processRightClick(position, worldManager, chunkRenderer, playerInventoryManager);
    }
}