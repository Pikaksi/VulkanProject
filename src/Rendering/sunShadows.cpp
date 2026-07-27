#include "sunShadows.hpp"

#include <iostream>

#include "VulkanTypes.hpp"
#include "vulkan/vulkan_core.h"

void createShadowDepthImage(VulkanCoreInfo& vulkanCoreInfo, ImageInfo shadowInfo)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = VK_FORMAT_D32_SFLOAT;
    imageInfo.extent = {2048, 2048, 1};
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(vulkanCoreInfo.device, &shadowInfo.image, nullptr, &shadowInfo.image) != VK_SUCCESS) {
        std::cerr << "failed to create shadow image" << std::endl;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vulkanCoreInfo.device, shadowInfo.image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(vulkanCoreInfo, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vulkanCoreInfo.device, &allocInfo, nullptr, &shadowInfo.memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(vulkanCoreInfo.device, shadowInfo.image, shadowInfo.memory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = shadowInfo.image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vulkanCoreInfo.device, &viewInfo, nullptr, &shadowInfo.view) != VK_SUCCESS) {
        std::cerr << "failed shadow image view\n";
    }
}
