#pragma once

#include "glm/vec3.hpp"
#include "FastNoise/FastNoise.h"

#include "World/Chunk.hpp"

const float frequency = 0.005;
const float heightNoiseMultiplier = 0.1f;

Chunk generateChunk(glm::i32vec3);