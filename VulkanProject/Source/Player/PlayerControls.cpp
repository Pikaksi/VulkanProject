#include "PlayerControls.hpp"

#include <iostream>

#include "PlaceBlock.hpp"
#include "PlayerInputHandler.hpp"

void updatePlayerControls(glm::vec3 position, WorldManager& worldManager, ChunkRenderer& chunkRenderer)
{
    if (PlayerInputHandler::getInstance().mouseRightPressed) {
        processInteracting(position, worldManager, chunkRenderer);
    }
}