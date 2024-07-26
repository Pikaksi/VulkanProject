#pragma once

#include <string>

#include "VulkanRendering/VulkanTypes.hpp"

const int BLOCK_TEXTURE_PIXEL_COUNT = 16;
const float UV_EDGE_CORRECTION = 1.0f / ((float)BLOCK_TEXTURE_PIXEL_COUNT * 2 * 16);

void createTextureImage(VulkanCoreInfo& vulkanCoreInfo, ImageInfo& imageInfo, VkCommandPool commandPool, bool generateMipLevels, std::string& fileName);
VkSampler createBlockTextureSampler(VulkanCoreInfo& vulkanCoreInfo);
VkSampler createUITextureSampler(VulkanCoreInfo& vulkanCoreInfo);
void createBlockTextureArray(VulkanCoreInfo& vulkanCoreInfo, ImageInfo& imageInfo, VkCommandPool commandPool, bool generateMipLevels);