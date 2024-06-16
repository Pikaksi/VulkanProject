#include "BlockTexCoordinateLookup.hpp"

#include <iostream>

// Takes .png file names from Textures/BlockTextures directory
// With 1 element in string vector means that all sides are that one image.
// With 3 elements element [0] = top, [1] = side, [2] = bottom.
// With 6 elements the order of the sides goes +x, -x, +y, -y, +z, -z.
inline const std::unordered_map<BlockType, std::vector<std::string>> blockTypeToFileNames = {
	{BlockType::stone, {"Stone"}},
	{BlockType::grass, {"GrassTop", "GrassSide", "Dirt"}},
	{BlockType::dirt, {"Dirt"}}
};

int findTexLayerFromTable(std::string fileName, std::unordered_map<std::string, int>& fileNameIndexTable)
{
	if (fileNameIndexTable.contains(fileName)) {
		return fileNameIndexTable.at(fileName);
	}
	std::cout << "Block texture not found!\n";
	return 0;
}

void generateBlockTexLayerLookupTable()
{
	std::unordered_map<std::string, int> fileNameIndexTable;
	int i = 0;

	// this loads the textures in the same way as the texture2dArray does
	for (auto const& dir_entry : std::filesystem::directory_iterator{ GetBlockTexturesDirPath() }) {
		if (dir_entry.path().extension() == ".png") {
			fileNameIndexTable.insert(std::pair<std::string, int>(dir_entry.path().stem().string(), i));
			i++;
		}
	}
	for (auto& blockTypeFiles : blockTypeToFileNames) {
		std::array<float, 6> texLayers;

		if (blockTypeFiles.second.size() == 1) {
			
			float imageTexLayer = findTexLayerFromTable(blockTypeFiles.second[0], fileNameIndexTable);

			for (int i = 0; i < 6; i++) {
				texLayers[i] = imageTexLayer;
			}
		}
		else if (blockTypeFiles.second.size() == 3) {

			float topImageTexLayer = findTexLayerFromTable(blockTypeFiles.second[0], fileNameIndexTable);
			float sideImageTexLayer = findTexLayerFromTable(blockTypeFiles.second[1], fileNameIndexTable);
			float bottomImageTexLayer = findTexLayerFromTable(blockTypeFiles.second[2], fileNameIndexTable);

			texLayers[0] = sideImageTexLayer;
			texLayers[1] = sideImageTexLayer;
			texLayers[2] = topImageTexLayer;
			texLayers[3] = bottomImageTexLayer;
			texLayers[4] = sideImageTexLayer;
			texLayers[5] = sideImageTexLayer;
		}
		else if (blockTypeFiles.second.size() == 6) {

			texLayers[0] = findTexLayerFromTable(blockTypeFiles.second[0], fileNameIndexTable);
			texLayers[1] = findTexLayerFromTable(blockTypeFiles.second[1], fileNameIndexTable);
			texLayers[2] = findTexLayerFromTable(blockTypeFiles.second[2], fileNameIndexTable);
			texLayers[3] = findTexLayerFromTable(blockTypeFiles.second[3], fileNameIndexTable);
			texLayers[4] = findTexLayerFromTable(blockTypeFiles.second[4], fileNameIndexTable);
			texLayers[5] = findTexLayerFromTable(blockTypeFiles.second[5], fileNameIndexTable);
		}
		else {
			throw std::runtime_error("Bad lenght in blockTypeToFileNames table!\n");
		}
		int8_t test = (int8_t)blockTypeFiles.first;
		std::cout << "inserted with " << test << "\n";
		for (int i = 0; i < 6; i++) {
			std::cout << texLayers[i] << "\n";
		}
		blockTypeToTexLayer.insert(std::make_pair(blockTypeFiles.first, texLayers));
	}
}