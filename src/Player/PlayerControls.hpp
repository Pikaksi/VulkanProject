#pragma once

#include <glm/vec3.hpp>

#include "World/WorldManager.hpp"
#include "3dRendering/ChunkRenderer.hpp"
#include "PlayerInventoryManager.hpp"
#include "playerInfo.hpp"
#include "CameraHandler.hpp"

void updatePlayerControls(glm::vec3 position,
                          WorldManager& worldManager,
                          BlockEntityManager& blockEntityManager,
                          ChunkRenderer& chunkRenderer,
                          CameraHandler& camerahandler,
                          PlayerInfo& playerInfo);
