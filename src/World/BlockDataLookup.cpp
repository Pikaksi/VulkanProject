#include "BlockDataLookup.hpp"

#include <cassert>
#include <stdexcept>
#include <filesystem>
#include <cmath>

#include "BlockType.hpp"
#include "Constants.hpp"
#include "FilePathHandler.hpp"
#include "assertm.hpp"
#include "blockEntity.hpp"

// clang-format off

// Takes .png file names from Textures/BlockTextures directory
// With 1 element in string vector means that all sides are that one image.
// With 2 elements element [0] = top and bottom, [1] = side
// With 3 elements element [0] = top, [1] = side, [2] = bottom.
// With 6 elements the order of the sides goes +x, -x, +y, -y, +z, -z.
const std::unordered_map<BlockType, std::vector<std::string>> blockTypeToFileNames = {
    {BlockType::stone, {"stone"}},
    {BlockType::grass, {"grassTop", "grassSide", "dirt"}},
    {BlockType::dirt, {"dirt"}},
    {BlockType::oakLog, {"oakLogTop", "oakLogSide"}},
    {BlockType::oakLeaf, {"oakLeafOpaque"}},
    {BlockType::grassPlant, {"grassPlant"}},
    {BlockType::furnace, {"furnaceTop", "furnaceSide"}},
    {BlockType::drill, {"drill"}},
    {BlockType::pipe, {"pipe"}},
    {BlockType::pipeIn, {"pipeIn"}},
    {BlockType::pipeOut, {"pipeOut"}},
};

const std::unordered_map<BlockType, std::vector<glm::vec3>> blockCustomRenderVertexOffsets = {
    {BlockType::grassPlant, {
        glm::vec3(0, 0, 0), glm::vec3(1, 0, 1), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
        glm::vec3(1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 1), glm::vec3(1, 1, 0),
        glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 1),
        glm::vec3(1, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 1, 1)
    }}
};

BlockRenderType blockTypeToRenderType[BlockType::maxEnum];
bool blockTypeIsInteractable[BlockType::maxEnum];
BlockEntityType blockTypeToBlockEntityType[BlockType::maxEnum];
std::unordered_map<Item, BlockType> itemToBlockType;

struct BlockProperties
{
    BlockType type;
    BlockRenderType renderType;
    bool isInteractable;
    BlockEntityType entityType;
    Item placedWithItem;
};

const BlockProperties blockPropertiesLookup[BlockType::maxEnum]
{
    BlockProperties {
        .type = BlockType::air,
        .renderType = BlockRenderType::dontRender,
        .isInteractable = false,
        .entityType = BlockEntityType::none,
        .placedWithItem = Item::empty,
    },
    BlockProperties {
        .type = BlockType::stone,
        .renderType = BlockRenderType::solid,
        .isInteractable = false,
        .entityType = BlockEntityType::none,
        .placedWithItem = Item::stone,
    },
    BlockProperties {
        .type = BlockType::grass,
        .renderType = BlockRenderType::solid,
        .isInteractable = false,
        .entityType = BlockEntityType::none,
        .placedWithItem = Item::grassBlock,
    },
    BlockProperties {
        .type = BlockType::dirt,
        .renderType = BlockRenderType::solid,
        .isInteractable = false,
        .entityType = BlockEntityType::none,
        .placedWithItem = Item::dirt,
    },
    BlockProperties {
        .type = BlockType::oakLog,
        .renderType = BlockRenderType::solid,
        .isInteractable = false,
        .entityType = BlockEntityType::none,
        .placedWithItem = Item::oakLog,
    },
    BlockProperties {
        .type = BlockType::oakLeaf,
        .renderType = BlockRenderType::solid,
        .isInteractable = false,
        .entityType = BlockEntityType::none,
        .placedWithItem = Item::empty,
    },
    BlockProperties {
        .type = BlockType::grassPlant,
        .renderType = BlockRenderType::custom,
        .isInteractable = false,
        .entityType = BlockEntityType::none,
        .placedWithItem = Item::empty,
    },
    BlockProperties {
        .type = BlockType::furnace,
        .renderType = BlockRenderType::solid,
        .isInteractable = true,
        .entityType = BlockEntityType::furnace,
        .placedWithItem = Item::furnaceBlock,
    },
    BlockProperties {
        .type = BlockType::drill,
        .renderType = BlockRenderType::solid,
        .isInteractable = true,
        .entityType = BlockEntityType::drill,
        .placedWithItem = Item::drillBlock,
    },
    BlockProperties {
        .type = BlockType::pipe,
        .renderType = BlockRenderType::solid,
        .isInteractable = false,
        .entityType = BlockEntityType::pipe,
        .placedWithItem = Item::pipeBlock,
    },
    BlockProperties {
        .type = BlockType::pipeIn,
        .renderType = BlockRenderType::solid,
        .isInteractable = false,
        .entityType = BlockEntityType::pipeIn,
        .placedWithItem = Item::pipeInBlock,
    },
    BlockProperties {
        .type = BlockType::pipeOut,
        .renderType = BlockRenderType::solid,
        .isInteractable = false,
        .entityType = BlockEntityType::pipeOut,
        .placedWithItem = Item::pipeOutBlock,
    },
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

bool getIsBlockEntity(BlockType blockType)
{
    return blockTypeToBlockEntityType[blockType] != BlockEntityType::none;
}

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

void initBlockImages()
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

void initBlockLookupArrays()
{
    for (int i = 0; i < (int)BlockType::maxEnum; i++) {
        blockTypeToRenderType[i] = blockPropertiesLookup[i].renderType;
    }
    for (int i = 0; i < (int)BlockType::maxEnum; i++) {
        blockTypeIsInteractable[i] = blockPropertiesLookup[i].isInteractable;
    }
    for (int i = 0; i < (int)BlockType::maxEnum; i++) {
        blockTypeToBlockEntityType[i] = blockPropertiesLookup[i].entityType;
    }
    for (int i = 0; i < (int)BlockType::maxEnum; i++) {
        itemToBlockType.insert(std::make_pair(blockPropertiesLookup[i].placedWithItem, (BlockType)i));
    }
}

void initBlockDataLookup()
{
    initBlockLookupArrays();
    initBlockImages();
}

void blockDataLookupCleanup()
{
    for (int i = 0; i < blockImages.size(); i++) {
        stbi_image_free(blockImages[i]);
    }
}
