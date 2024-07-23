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
	{BlockType::grassPlant, {"GrassPlant"}}
};

const std::unordered_map<BlockType, std::vector<glm::vec3>> blockCustomRenderVertexOffsets = {
	{BlockType::grassPlant, {
		glm::vec3(0, 0, 0), glm::vec3(1, 0, 1), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
		glm::vec3(1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 1), glm::vec3(1, 1, 0),
		glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 1),
		glm::vec3(1, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 1, 1)
	}}
};

bool isBlockSolid(BlockType blocktype)
{
	return blockRenderType[blocktype] == BlockRenderType::renderSolidBlock;
}

bool isRenderableNonSolid(BlockType blocktype)
{
	return !(blockRenderType[blocktype] == BlockRenderType::renderSolidBlock || blockRenderType[blocktype] == BlockRenderType::dontRender);
}

BlockRenderType getRenderType(BlockType blockType) {
	return blockRenderType[blockType];
}

BlockRenderType getBlockRenderType(BlockType blockType)
{
	return blockRenderType[blockType];
}