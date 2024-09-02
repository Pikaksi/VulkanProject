#include "BlockDataLookup.hpp"

#include <stdexcept>

// Takes .png file names from Textures/BlockTextures directory
// With 1 element in string vector means that all sides are that one image.
// With 2 elements element [0] = top and bottom, [1] = side
// With 3 elements element [0] = top, [1] = side, [2] = bottom.
// With 6 elements the order of the sides goes +x, -x, +y, -y, +z, -z.
const std::unordered_map<BlockType, std::vector<std::string>> blockTypeToFileNames = {
	{BlockType::stone, {"Stone"}},
	{BlockType::grass, {"GrassTop", "GrassSide", "Dirt"}},
	{BlockType::dirt, {"Dirt"}},
	{BlockType::oakLog, {"OakLogTop", "OakLogSide"}},
	{BlockType::oakLeaf, {"OakLeafOpaque"}},
	{BlockType::grassPlant, {"GrassPlant"}},
	{BlockType::furnace, {"FurnaceTop", "FurnaceSide"}}
};

const std::unordered_map<BlockType, std::vector<glm::vec3>> blockCustomRenderVertexOffsets = {
	{BlockType::grassPlant, {
		glm::vec3(0, 0, 0), glm::vec3(1, 0, 1), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
		glm::vec3(1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 1), glm::vec3(1, 1, 0),
		glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 1),
		glm::vec3(1, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 1, 1)
	}}
};

const BlockProperties blockPropertiesLookup[BlockType::maxEnum]
{
	BlockProperties {
		.blockType = BlockType::air,
		.blockRenderType = BlockRenderType::dontRender,
		.isInteractable = false,
		.blockComponents = 0
	},
	BlockProperties {
		.blockType = BlockType::stone,
		.blockRenderType = BlockRenderType::solid,
		.isInteractable = false,
		.blockComponents = 0
	},
	BlockProperties {
		.blockType = BlockType::grass,
		.blockRenderType = BlockRenderType::solid,
		.isInteractable = false,
		.blockComponents = 0
	},
	BlockProperties {
		.blockType = BlockType::dirt,
		.blockRenderType = BlockRenderType::solid,
		.isInteractable = false,
		.blockComponents = 0
	},
	BlockProperties {
		.blockType = BlockType::oakLog,
		.blockRenderType = BlockRenderType::solid,
		.isInteractable = false,
		.blockComponents = 0
	},
	BlockProperties {
		.blockType = BlockType::oakLeaf,
		.blockRenderType = BlockRenderType::solid,
		.isInteractable = false,
		.blockComponents = 0
	},
	BlockProperties {
		.blockType = BlockType::grassPlant,
		.blockRenderType = BlockRenderType::custom,
		.isInteractable = false,
		.blockComponents = 0
	},
	BlockProperties {
		.blockType = BlockType::furnace,
		.blockRenderType = BlockRenderType::solid,
		.isInteractable = true,
		.blockComponents = inventoryComponentBitmask
	}
};

bool isBlockSolid(BlockType blocktype)
{
	return blockRenderTypes[blocktype] == BlockRenderType::solid;
}

bool isRenderableNonSolid(BlockType blocktype)
{
	return !(blockRenderTypes[blocktype] == BlockRenderType::solid || blockRenderTypes[blocktype] == BlockRenderType::dontRender);
}

BlockRenderType getRenderType(BlockType blockType) {
	return blockRenderTypes[blockType];
}

BlockRenderType getBlockRenderType(BlockType blockType)
{
	return blockRenderTypes[blockType];
}

const BlockRenderType blockRenderTypes[BlockType::maxEnum] = {
	blockPropertiesLookup[0].blockRenderType,
	blockPropertiesLookup[1].blockRenderType,
	blockPropertiesLookup[2].blockRenderType,
	blockPropertiesLookup[3].blockRenderType,
	blockPropertiesLookup[4].blockRenderType,
	blockPropertiesLookup[5].blockRenderType,
	blockPropertiesLookup[6].blockRenderType,
	blockPropertiesLookup[7].blockRenderType
};

const bool blockIsInteractable[BlockType::maxEnum] = {
	blockPropertiesLookup[0].isInteractable,
	blockPropertiesLookup[1].isInteractable,
	blockPropertiesLookup[2].isInteractable,
	blockPropertiesLookup[3].isInteractable,
	blockPropertiesLookup[4].isInteractable,
	blockPropertiesLookup[5].isInteractable,
	blockPropertiesLookup[6].isInteractable,
	blockPropertiesLookup[7].isInteractable
};

const uint64_t blockComponents[BlockType::maxEnum] = {
	blockPropertiesLookup[0].blockComponents,
	blockPropertiesLookup[1].blockComponents,
	blockPropertiesLookup[2].blockComponents,
	blockPropertiesLookup[3].blockComponents,
	blockPropertiesLookup[4].blockComponents,
	blockPropertiesLookup[5].blockComponents,
	blockPropertiesLookup[6].blockComponents,
	blockPropertiesLookup[7].blockComponents
};
