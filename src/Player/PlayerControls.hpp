#pragma once

#include <glm/vec3.hpp>

#include "World/WorldManager.hpp"
#include "3dRendering/ChunkRenderer.hpp"
#include "PlayerInventoryManager.hpp"
#include "playerInfo.hpp"

void updatePlayerControls(glm::vec3 position,
                          WorldManager& worldManager,
                          BlockEntityManager& blockEntityManager,
                          ChunkRenderer& chunkRenderer,
                          PlayerInfo& playerInfo);
