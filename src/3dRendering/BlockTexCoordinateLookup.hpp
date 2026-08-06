#pragma once

#include <unordered_map>
#include <array>

#include "World/BlockType.hpp"
#include "FilePathHandler.hpp"

void generateBlockTextureLayerLookupTable();
// Side order is right, left, up, down, forward, backward
float getBlockTextureLayer(BlockType block, int side);
