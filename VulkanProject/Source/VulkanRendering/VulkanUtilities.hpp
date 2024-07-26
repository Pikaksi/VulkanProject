#pragma once

#include "vulkan/vulkan.h"

#include "VulkanTypes.hpp"

uint32_t findMemoryType(VulkanCoreInfo& vulkanCoreInfo, uint32_t typeFilter, VkMemoryPropertyFlags properties);
VkFormat findSupportedFormat(VulkanCoreInfo& vulkanCoreInfo, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
void readFile(const std::string& filename, std::vector<char>& buffer);
bool hasStencilComponent(VkFormat format);