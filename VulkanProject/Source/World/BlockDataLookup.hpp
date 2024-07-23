#pragma once

#include <numbers>
#include <unordered_map>
#include <string>

#include <glm/vec3.hpp>

#include "BlockType.hpp"

enum class BlockRenderType : uint8_t
{
	dontRender = 0,
	renderSolidBlock = 1,
	renderTransparentBlock = 2,
	renderCustom = 3
};

const BlockRenderType blockRenderType[7]{
	BlockRenderType::dontRender,
	BlockRenderType::renderSolidBlock,
	BlockRenderType::renderSolidBlock,
	BlockRenderType::renderSolidBlock,
	BlockRenderType::renderSolidBlock,
	BlockRenderType::renderSolidBlock,
	BlockRenderType::renderCustom
};

extern const std::unordered_map<BlockType, std::vector<std::string>> blockTypeToFileNames;
extern const std::unordered_map<BlockType, std::vector<glm::vec3>> blockCustomRenderVertexOffsets;
bool isBlockSolid(BlockType blocktype);
BlockRenderType getRenderType(BlockType blockType);
bool isRenderableNonSolid(BlockType blocktype);
BlockRenderType getBlockRenderType(BlockType blockType);
