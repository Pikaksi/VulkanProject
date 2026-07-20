#include "QuadStripIndexBuffer.hpp"

#include <vector>

#include "VulkanRendering/Buffers.hpp"

QuadStripIndexBuffer::QuadStripIndexBuffer(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, uint32_t quadCount) :
	quadCount(quadCount), indexCount(quadCount * 6)
{
    std::vector<uint32_t> indices;
    for (int i = 0; i < quadCount; i++) {
        indices.push_back(i * 4);
        indices.push_back(i * 4 + 1);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 3);
        indices.push_back(i * 4);
    }

    createIndexBuffer(vulkanCoreInfo, commandPool, buffer, bufferMemory, indices);
}

void QuadStripIndexBuffer::cleanUp(VulkanCoreInfo& vulkanCoreInfo)
{
    vkDestroyBuffer(vulkanCoreInfo.device, buffer, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, bufferMemory, nullptr);
}

VkBuffer& QuadStripIndexBuffer::getBuffer()
{
    return buffer;
}

uint32_t QuadStripIndexBuffer::getIndexCount()
{
    return indexCount;
}

uint32_t QuadStripIndexBuffer::getVertexCount()
{
    return quadCount * 4;
}