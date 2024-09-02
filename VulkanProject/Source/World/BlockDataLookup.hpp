#pragma once

#include <numbers>
#include <unordered_map>
#include <string>

#include <glm/vec3.hpp>

#include "BlockType.hpp"
#include "ECS/Components.hpp"

enum class BlockRenderType : uint8_t
{
	dontRender = 0,
	solid = 1,
	transparent = 2,
	custom = 3
};

extern const BlockRenderType blockTypeRenderType[BlockType::maxEnum];
extern const bool blockTypeIsInteractable[BlockType::maxEnum];
extern const uint64_t blockTypeComponents[BlockType::maxEnum];

extern const std::unordered_map<BlockType, std::vector<std::string>> blockTypeToFileNames;
extern const std::unordered_map<BlockType, std::vector<glm::vec3>> blockCustomRenderVertexOffsets;
bool isBlockSolid(BlockType blocktype);
BlockRenderType getRenderType(BlockType blockType);
bool isRenderableNonSolid(BlockType blocktype);
BlockRenderType getBlockRenderType(BlockType blockType);
