#pragma once

#include "VulkanTypes.hpp"

void createImage(VulkanCoreInfo* vulkanCoreInfo, ImageInfo* imageInfo, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
VkImageView createImageView(VulkanCoreInfo* vulkanCoreInfo, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

void createImageInfo(VulkanCoreInfo* vulkanCoreInfo,
    ImageInfo* imageInfo,
    uint32_t width,
    uint32_t height,
    uint32_t mipLevels,
    VkSampleCountFlagBits numSamples,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImageAspectFlags aspectFlags);
