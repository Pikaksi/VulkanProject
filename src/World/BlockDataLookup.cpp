#include "BlockDataLookup.hpp"

#include <cassert>
#include <stdexcept>
#include <filesystem>
#include <cmath>

#include "Constants.hpp"
#include "FilePathHandler.hpp"
#include "ECS/Components.hpp"
#include "assertm.hpp"

// clang-format off

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

struct BlockProperties
{
    BlockType blockType;
    BlockRenderType blockRenderType;
    bool isInteractable;
    uint64_t blockComponents;
};

const BlockProperties blockPropertiesLookup[BlockType::maxEnum]
{
    BlockProperties {
        .blockType = BlockType::air,
        .blockRenderType = BlockRenderType::dontRender,
        .isInteractable = false,
        .blockComponents = 0,
    },
    BlockProperties {
        .blockType = BlockType::stone,
        .blockRenderType = BlockRenderType::solid,
        .isInteractable = false,
        .blockComponents = 0,
    },
    BlockProperties {
        .blockType = BlockType::grass,
        .blockRenderType = BlockRenderType::solid,
        .isInteractable = false,
        .blockComponents = 0,
    },
    BlockProperties {
        .blockType = BlockType::dirt,
        .blockRenderType = BlockRenderType::solid,
        .isInteractable = false,
        .blockComponents = 0,
    },
    BlockProperties {
        .blockType = BlockType::oakLog,
        .blockRenderType = BlockRenderType::solid,
        .isInteractable = false,
        .blockComponents = 0,
    },
    BlockProperties {
        .blockType = BlockType::oakLeaf,
        .blockRenderType = BlockRenderType::solid,
        .isInteractable = false,
        .blockComponents = 0,
    },
    BlockProperties {
        .blockType = BlockType::grassPlant,
        .blockRenderType = BlockRenderType::custom,
        .isInteractable = false,
        .blockComponents = 0,
    },
    BlockProperties {
        .blockType = BlockType::furnace,
        .blockRenderType = BlockRenderType::solid,
        .isInteractable = true,
        .blockComponents = inventoryComponentBitmask,
    }
};

const std::map<BlockType, int> blockTypeInventorySize
{
    {BlockType::furnace, 2}
};

bool isBlockSolid(BlockType blocktype)
{
    return blockTypeToRenderType[blocktype] == BlockRenderType::solid;
}

bool isRenderableNonSolid(BlockType blocktype)
{
    return !(blockTypeToRenderType[blocktype] == BlockRenderType::solid || blockTypeToRenderType[blocktype] == BlockRenderType::dontRender);
}

BlockRenderType getRenderType(BlockType blockType) {
    return blockTypeToRenderType[blockType];
}

BlockRenderType getBlockRenderType(BlockType blockType)
{
    return blockTypeToRenderType[blockType];
}

bool blockHasComponent(BlockType blockType, uint64_t componentBitmask)
{
    return blockTypeToComponents[blockType] & componentBitmask != 0;
}

bool blockHasComponent(BlockType blockType)
{
    return blockTypeToComponents[blockType] != 0;
}

const BlockRenderType blockTypeToRenderType[BlockType::maxEnum] = {
    blockPropertiesLookup[0].blockRenderType,
    blockPropertiesLookup[1].blockRenderType,
    blockPropertiesLookup[2].blockRenderType,
    blockPropertiesLookup[3].blockRenderType,
    blockPropertiesLookup[4].blockRenderType,
    blockPropertiesLookup[5].blockRenderType,
    blockPropertiesLookup[6].blockRenderType,
    blockPropertiesLookup[7].blockRenderType
};

const bool blockTypeIsInteractable[BlockType::maxEnum] = {
    blockPropertiesLookup[0].isInteractable,
    blockPropertiesLookup[1].isInteractable,
    blockPropertiesLookup[2].isInteractable,
    blockPropertiesLookup[3].isInteractable,
    blockPropertiesLookup[4].isInteractable,
    blockPropertiesLookup[5].isInteractable,
    blockPropertiesLookup[6].isInteractable,
    blockPropertiesLookup[7].isInteractable
};

const uint64_t blockTypeToComponents[BlockType::maxEnum] = {
    blockPropertiesLookup[0].blockComponents,
    blockPropertiesLookup[1].blockComponents,
    blockPropertiesLookup[2].blockComponents,
    blockPropertiesLookup[3].blockComponents,
    blockPropertiesLookup[4].blockComponents,
    blockPropertiesLookup[5].blockComponents,
    blockPropertiesLookup[6].blockComponents,
    blockPropertiesLookup[7].blockComponents
};

// clang-format on

glm::vec3 calculateImageColorInLinearSpace(stbi_uc* image, int height, int width)
{
    double sumR = 0, sumG = 0, sumB = 0, sumA = 0;
    int pixels = width * height;
    for (int i = 0; i < pixels; i++) {
        double r = image[4 * i];
        double g = image[4 * i + 1];
        double b = image[4 * i + 2];

        double a = image[4 * i + 3] / 255.0;

        sumR += std::pow(r / 255.0, 2.2) * a;
        sumG += std::pow(g / 255.0, 2.2) * a;
        sumB += std::pow(b / 255.0, 2.2) * a;
        sumA += a;
    }
    if (sumA == 0)
        return glm::vec3{0.0f, 0.0f, 0.0f};
    glm::vec3 color{sumR / sumA, sumG / sumA, sumB / sumA};
    return color;
}

void blockDataLookupInit()
{
    std::unordered_map<std::string, uint32_t> fileNameToImageIndex;
    uint32_t i = 0;

    for (auto const& dir_entry : std::filesystem::directory_iterator{GetBlockTexturesDirPath()}) {
        if (dir_entry.path().extension() == ".png") {
            std::string path = GetBlockTexturesDirPath() + "/" + dir_entry.path().filename().string();
            int width = 0;
            int height = 0;
            int texChannels; // Unused.

            stbi_uc* imageData = stbi_load(path.c_str(), &width, &height, &texChannels, STBI_rgb_alpha);

            assertm(imageData != nullptr, "Image loader gave nullptr to image " << path);
            assertm(width == BLOCK_TEXTURE_PIXEL_COUNT && height == BLOCK_TEXTURE_PIXEL_COUNT,
                    "Block texture at " << path << " has the wrong size.");

            fileNameToImageIndex.insert(std::pair<std::string, uint32_t>(dir_entry.path().stem().string(), i));
            i += 1;
            blockImages.push_back(imageData);
        }
    }
    for (auto& blockTypeFiles : blockTypeToFileNames) {
        for (auto& fileName : blockTypeFiles.second) {
            assertm(fileNameToImageIndex.contains(fileName),
                    "Could not find requested file " << fileName << " for block " << blockTypeFiles.first);
        }
    }

    for (auto& blockTypeFiles : blockTypeToFileNames) {
        uint32_t* texLayers = blockTypeToTexLayer[blockTypeFiles.first];
        BlockType block = blockTypeFiles.first;
        const std::vector<std::string>& fileNames = blockTypeFiles.second;

        if (fileNames.size() == 1) {
            uint32_t imageTexLayer = fileNameToImageIndex.at(fileNames[0]);

            for (int i = 0; i < 6; i++) {
                texLayers[i] = imageTexLayer;
            }
        }
        else if (blockTypeFiles.second.size() == 2) {
            uint32_t topAndBottomImageTexLayer = fileNameToImageIndex.at(fileNames[0]);
            uint32_t sideImageTexLayer = fileNameToImageIndex.at(fileNames[1]);

            texLayers[0] = sideImageTexLayer;
            texLayers[1] = sideImageTexLayer;
            texLayers[2] = topAndBottomImageTexLayer;
            texLayers[3] = topAndBottomImageTexLayer;
            texLayers[4] = sideImageTexLayer;
            texLayers[5] = sideImageTexLayer;
        }
        else if (blockTypeFiles.second.size() == 3) {
            uint32_t topImageTexLayer = fileNameToImageIndex.at(fileNames[0]);
            uint32_t sideImageTexLayer = fileNameToImageIndex.at(fileNames[1]);
            uint32_t bottomImageTexLayer = fileNameToImageIndex.at(fileNames[2]);

            texLayers[0] = sideImageTexLayer;
            texLayers[1] = sideImageTexLayer;
            texLayers[2] = topImageTexLayer;
            texLayers[3] = bottomImageTexLayer;
            texLayers[4] = sideImageTexLayer;
            texLayers[5] = sideImageTexLayer;
        }
        else if (blockTypeFiles.second.size() == 6) {

            texLayers[0] = fileNameToImageIndex.at(fileNames[0]);
            texLayers[1] = fileNameToImageIndex.at(fileNames[1]);
            texLayers[2] = fileNameToImageIndex.at(fileNames[2]);
            texLayers[3] = fileNameToImageIndex.at(fileNames[3]);
            texLayers[4] = fileNameToImageIndex.at(fileNames[4]);
            texLayers[5] = fileNameToImageIndex.at(fileNames[5]);
        }
        else {
            assertm(false, "Bad lenght in blockTypeToFileNames table");
        }
    }

    for (stbi_uc* image : blockImages) {
        glm::vec3 color = calculateImageColorInLinearSpace(image, BLOCK_TEXTURE_PIXEL_COUNT, BLOCK_TEXTURE_PIXEL_COUNT);
        blockImageColors.push_back(color);
    }
}
