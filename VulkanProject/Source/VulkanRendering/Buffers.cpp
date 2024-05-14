#include <stdexcept>

#include "Buffers.hpp"
#include "VulkanUtilities.hpp"
#include "Constants.hpp"
#include "Commands.hpp"

void createBuffer(VulkanCoreInfo* vulkanCoreInfo, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkanCoreInfo->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkanCoreInfo->device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(vulkanCoreInfo, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vulkanCoreInfo->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(vulkanCoreInfo->device, buffer, bufferMemory, 0);
}

void copyBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(vulkanCoreInfo, commandPool);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(vulkanCoreInfo, commandPool, commandBuffer);
}

void copyBufferToImage(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(vulkanCoreInfo, commandPool);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(vulkanCoreInfo, commandPool, commandBuffer);
}

void createUniformBuffer(VulkanCoreInfo* vulkanCoreInfo, UniformBufferInfo* uniformBufferInfo) {
    VkDeviceSize bufferSize = sizeof(CameraUniformBufferObject);

    createBuffer(vulkanCoreInfo, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBufferInfo->buffer, uniformBufferInfo->memory);

    vkMapMemory(vulkanCoreInfo->device, uniformBufferInfo->memory, 0, bufferSize, 0, &uniformBufferInfo->mappingPointer);
}

void createCameraUniformBuffers(VulkanCoreInfo* vulkanCoreInfo, std::vector<UniformBufferInfo*> cameraUBOs)
{
    cameraUBOs.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createUniformBuffer(vulkanCoreInfo, cameraUBOs[i]);
    }
}