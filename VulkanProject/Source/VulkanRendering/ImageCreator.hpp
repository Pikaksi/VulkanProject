#pragma once

#include "VulkanTypes.hpp"

void createImage(VulkanCoreInfo* vulkanCoreInfo, ImageInfo* imageInfo, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
void fillImageView(VulkanCoreInfo* vulkanCoreInfo, ImageInfo* imageInfo, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
void transitionImageLayout(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
void generateMipmaps(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

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
