#pragma once

#include <glm/vec3.hpp>

#include "World/WorldManager.hpp"
#include "World/BlockType.hpp"
#include "3dRendering/ChunkRenderer.hpp"
#include "Player/PlayerInventoryManager.hpp"
#include "blockEntityManager.hpp"

void placeBlock(glm::ivec3 chunkLocation,
                glm::ivec3 blockLocation,
                BlockType blockToPlace,
                WorldManager& worldManager,
                BlockEntityManager& blockEntityManager,
                ChunkRenderer& chunkRenderer);
