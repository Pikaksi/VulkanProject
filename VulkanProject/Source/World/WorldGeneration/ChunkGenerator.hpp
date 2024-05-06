#pragma once

#include "glm/vec3.hpp"
#include "FastNoise/FastNoise.h"

#include "World/Chunk.hpp"

const float frequency = 0.3f;

Chunk generateChunk(glm::i32vec3);