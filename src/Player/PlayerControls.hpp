#pragma once

#include <glm/vec3.hpp>

#include "World/WorldManager.hpp"
#include "3dRendering/ChunkRenderer.hpp"
#include "PlayerInventoryManager.hpp"

void updatePlayerControls(
    glm::vec3 position,
    WorldManager& worldManager,
    ChunkRenderer& chunkRenderer,
    PlayerInventoryManager& playerInventoryManager);