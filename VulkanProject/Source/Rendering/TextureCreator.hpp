#pragma once

#include <string>

#include "VulkanRendering/VulkanTypes.hpp"

void createTextureImage(VulkanCoreInfo* vulkanCoreInfo, ImageInfo* imageInfo, VkCommandPool commandPool, bool generateMipLevels, std::string fileName);
VkSampler createBlockTextureSampler(VulkanCoreInfo* vulkanCoreInfo);
VkSampler createTextTextureSampler(VulkanCoreInfo* vulkanCoreInfo);
void createBlockTextureArray(VulkanCoreInfo* vulkanCoreInfo, ImageInfo* imageInfo, VkCommandPool commandPool, bool generateMipLevels);