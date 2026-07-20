#pragma once

#include <glm/vec3.hpp>

#include "World/WorldManager.hpp"
#include "World/BlockType.hpp"
#include "3dRendering/ChunkRenderer.hpp"
#include "Player/PlayerInventoryManager.hpp"

void processRightClick(
    glm::vec3 position,
    WorldManager& worldManager,
    ChunkRenderer& chunkRenderer,
    PlayerInventoryManager& playerInventoryManager);