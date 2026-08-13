#pragma once

#include <numbers>
#include <unordered_map>
#include <map>
#include <string>
#include <vector>

#include "stb_image.h"
#include <glm/vec3.hpp>

#include "BlockType.hpp"

enum class BlockRenderType : uint8_t
{
    dontRender = 0,
    solid = 1,
    transparent = 2,
    custom = 3
};

extern const BlockRenderType blockTypeToRenderType[BlockType::maxEnum];
extern const bool blockTypeIsInteractable[BlockType::maxEnum];
extern const uint64_t blockTypeToComponents[BlockType::maxEnum];

inline std::vector<stbi_uc*> blockImages;
inline std::vector<glm::vec3> blockImageColors;
inline uint32_t blockTypeToTexLayer[BlockType::maxEnum][6];

extern const std::unordered_map<BlockType, std::vector<std::string>> blockTypeToFileNames;
extern const std::unordered_map<BlockType, std::vector<glm::vec3>> blockCustomRenderVertexOffsets;
extern const std::map<BlockType, int> blockTypeInventorySize;

void blockDataLookupInit();

bool isBlockSolid(BlockType blocktype);
BlockRenderType getRenderType(BlockType blockType);
bool isRenderableNonSolid(BlockType blocktype);
BlockRenderType getBlockRenderType(BlockType blockType);
bool blockHasComponent(BlockType blockType, uint64_t componentBitmask);
bool blockHasComponent(BlockType blockType);
