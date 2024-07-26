#pragma once

#include <string>
#include <map>

#include "VulkanRendering/VulkanTypes.hpp"

enum class UITexture
{
	text = 0,
	whiteBox = 1
};

const std::map<int, std::string> textureToFileName = {
	{(int)UITexture::text, "TextSpriteSheet"},
	{(int)UITexture::whiteBox, "WhiteBox"}
};

void createUIImageInfos(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, std::vector<ImageInfo>& uiImageInfos);