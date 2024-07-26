#pragma once

#include "VulkanTypes.hpp"

VkCommandBuffer beginSingleTimeCommands(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool);
void endSingleTimeCommands(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, VkCommandBuffer commandBuffer);
VkCommandPool createCommandPool(VulkanCoreInfo& vulkanCoreInfo);
std::vector<VkCommandBuffer> createCommandBuffers(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool);