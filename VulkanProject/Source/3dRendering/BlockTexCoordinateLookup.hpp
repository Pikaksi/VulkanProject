#pragma once

#include <filesystem>
#include <unordered_map>
#include <array>

#include "World/BlockType.hpp"
#include "FilePathHandler.hpp"

void generateBlockTexLayerLookupTable();

inline std::unordered_map<BlockType, std::array<float, 6>> blockTypeToTexLayer;