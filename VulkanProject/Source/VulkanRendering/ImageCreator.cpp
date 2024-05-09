#include <stdexcept>

#include "ImageCreator.hpp"
#include "VulkanUtilities.hpp"

VkImageView createImageView(VulkanCoreInfo* vulkanCoreInfo, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(vulkanCoreInfo->device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

void createImage(VulkanCoreInfo* vulkanCoreInfo, ImageInfo* imageInfo, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
{    
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = mipLevels;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = usage;
    imageCreateInfo.samples = numSamples;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(vulkanCoreInfo->device, &imageCreateInfo, nullptr, &imageInfo->image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vulkanCoreInfo->device, imageInfo->image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(vulkanCoreInfo, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vulkanCoreInfo->device, &allocInfo, nullptr, &imageInfo->memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(vulkanCoreInfo->device, imageInfo->image, imageInfo->memory, 0);
}

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
                     VkImageAspectFlags aspectFlags)
{
    createImage(vulkanCoreInfo, imageInfo, width, height, mipLevels, numSamples, format, tiling, usage, properties);
    imageInfo->view = createImageView(vulkanCoreInfo, imageInfo->image, format, aspectFlags, mipLevels);
}