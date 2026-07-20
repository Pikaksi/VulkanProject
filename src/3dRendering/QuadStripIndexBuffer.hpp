#pragma once

#include "VulkanRendering/VulkanTypes.hpp"

class QuadStripIndexBuffer
{
public:
	QuadStripIndexBuffer() {}
	QuadStripIndexBuffer(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, uint32_t quadCount);
	void cleanUp(VulkanCoreInfo& vulkanCoreInfo);
	VkBuffer& getBuffer();
	uint32_t getIndexCount();
	uint32_t getVertexCount();

private:
	uint32_t quadCount;
	uint32_t indexCount;
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
};