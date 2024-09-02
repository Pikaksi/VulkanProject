#pragma once

#include <glm/vec3.hpp>

#include "World/WorldManager.hpp"
#include "World/BlockType.hpp"
#include "3dRendering/ChunkRenderer.hpp"

void processInteracting(glm::vec3 position, WorldManager& worldManager, ChunkRenderer& chunkRenderer);