#pragma once

#include "VulkanRendering/VulkanTypes.hpp"

void createTextureImage(VulkanCoreInfo* vulkanCoreInfo, ImageInfo* imageInfo, VkCommandPool commandPool, bool generateMipLevels);
VkSampler createTextureSampler(VulkanCoreInfo* vulkanCoreInfo);