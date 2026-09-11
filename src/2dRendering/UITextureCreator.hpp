#pragma once

#include <string>
#include <map>

#include "UITexLayer.hpp"
#include "VulkanRendering/VulkanTypes.hpp"

// clang-format off

// ItemToTexLayer.hpp contains information for which item uses which tex layer
const std::map<int, std::string> textureToFileName = {
    {(int)UITexLayer::text, "TextSpriteSheet"},
    {(int)UITexLayer::white, "WhiteBox"},
    {(int)UITexLayer::itemStick, "Stick"},
    {(int)UITexLayer::itemOakPlank, "oakPlank"},
    {(int)UITexLayer::itemStone, "Stone"},
    {(int)UITexLayer::itemOakLog, "OakLog"},
    {(int)UITexLayer::itemOakLeaf, "OakLeaf"},
    {(int)UITexLayer::itemDirt, "Dirt"},
    {(int)UITexLayer::itemGrassBlock, "GrassBlock"},
    {(int)UITexLayer::itemFurnace, "FurnaceBlock"},
    {(int)UITexLayer::itemDrillBlock, "drillBlock"},
    {(int)UITexLayer::itemPipeBlock, "pipe"},
    {(int)UITexLayer::itemPipeInBlock, "pipeIn"},
    {(int)UITexLayer::itemPipeOutBlock, "pipeOut"},
};
// clang-format on

void createUIImageInfos(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, std::vector<ImageInfo>& uiImageInfos);
