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

struct BlockProperties
{
	BlockType blockType;
	BlockRenderType blockRenderType;
	bool isInteractable;
	uint64_t blockComponents;
};

extern BlockProperties blockPropertiesLookup[BlockType::maxEnum];

const BlockRenderType blockRenderType[8] {
	BlockRenderType::dontRender,
	BlockRenderType::solid,
	BlockRenderType::solid,
	BlockRenderType::solid,
	BlockRenderType::solid,
	BlockRenderType::solid,
	BlockRenderType::custom,
	BlockRenderType::solid
};

extern const std::unordered_map<BlockType, std::vector<std::string>> blockTypeToFileNames;
extern const std::unordered_map<BlockType, std::vector<glm::vec3>> blockCustomRenderVertexOffsets;
bool isBlockSolid(BlockType blocktype);
BlockRenderType getRenderType(BlockType blockType);
bool isRenderableNonSolid(BlockType blocktype);
BlockRenderType getBlockRenderType(BlockType blockType);
