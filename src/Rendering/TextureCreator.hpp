#pragma once

#include <string>

#include "VulkanRendering/VulkanTypes.hpp"
#include "BlockType.hpp"

void createTextureImage(VulkanCoreInfo& vulkanCoreInfo, ImageInfo& imageInfo, VkCommandPool commandPool, bool generateMipLevels, std::string& textureFilePath);
VkSampler createBlockTextureSampler(VulkanCoreInfo& vulkanCoreInfo);
VkSampler createSunShadowSampler(VulkanCoreInfo& vulkanCoreInfo);
VkSampler createUITextureSampler(VulkanCoreInfo& vulkanCoreInfo);
void createBlockTextureArray(VulkanCoreInfo& vulkanCoreInfo, ImageInfo& imageInfo, VkCommandPool commandPool, bool generateMipLevels);
