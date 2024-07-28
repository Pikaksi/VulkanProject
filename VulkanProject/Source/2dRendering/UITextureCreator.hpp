#pragma once

#include <string>
#include <map>

#include "UITexLayer.hpp"
#include "VulkanRendering/VulkanTypes.hpp"

const std::map<int, std::string> textureToFileName = {
	{(int)UITexLayer::text, "TextSpriteSheet"},
	{(int)UITexLayer::white, "WhiteBox"}
};

void createUIImageInfos(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, std::vector<ImageInfo>& uiImageInfos);