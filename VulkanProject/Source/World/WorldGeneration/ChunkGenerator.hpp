#pragma once

#include "glm/vec3.hpp"

#include <unordered_set>

#include "World/WorldManager.hpp"
#include "World/Chunk.hpp"

const float frequency = 0.005;
const float heightNoiseMultiplier = 0.1f;

void generateChunk(glm::i32vec3, WorldManager* worldManager, std::unordered_set<glm::ivec3>& chunksToRerender);