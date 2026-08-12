#pragma once

#include "glm/vec3.hpp"

#include <unordered_set>

#include "ChunkRenderer.hpp"
#include "World/WorldManager.hpp"
#include "World/Chunk.hpp"

const float frequency = 1.0;
const float heightNoiseMultiplier = 0.1f;

void generateChunk(glm::i32vec3, WorldManager* worldManager, std::vector<glm::ivec3>& chunksToRerender, ChunkRenderer& chunkRenderer);
