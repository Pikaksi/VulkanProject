#pragma once

#include "VulkanTypes.hpp"

void createImage(VulkanCoreInfo* vulkanCoreInfo, ImageInfo* imageInfo, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t arrayLayers);
void fillImageView(VulkanCoreInfo* vulkanCoreInfo, VkImage image, VkImageView& imageView, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t arrayLayers, VkImageViewType imageViewType);
void transitionImageLayout(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount);
void generateMipmaps(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, uint32_t layerCount);

void createImageInfo(
    VulkanCoreInfo* vulkanCoreInfo,
    ImageInfo* imageInfo,
    uint32_t width,
    uint32_t height,
    uint32_t mipLevels,
    VkSampleCountFlagBits numSamples,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImageAspectFlags aspectFlags,
    uint32_t arrayLayers,
    VkImageViewType imageViewType);
