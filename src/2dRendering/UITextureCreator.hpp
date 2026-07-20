#pragma once

#include <string>
#include <map>

#include "UITexLayer.hpp"
#include "VulkanRendering/VulkanTypes.hpp"

const std::map<int, std::string> textureToFileName = {
	{(int)UITexLayer::text, "TextSpriteSheet"},
	{(int)UITexLayer::white, "WhiteBox"},
	{(int)UITexLayer::itemStick, "Stick"},
	{(int)UITexLayer::itemStone, "Stone"},
	{(int)UITexLayer::itemOakLog, "OakLog"},
	{(int)UITexLayer::itemOakLeaf, "OakLeaf"},
	{(int)UITexLayer::itemDirt, "Dirt"},
	{(int)UITexLayer::itemGrassBlock, "GrassBlock"},
};

void createUIImageInfos(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, std::vector<ImageInfo>& uiImageInfos);