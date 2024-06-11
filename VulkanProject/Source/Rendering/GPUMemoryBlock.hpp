#pragma once

#include "vulkan/vulkan.h"

#include <vector>
#include <stdexcept>

#include "VulkanRendering/VulkanTypes.hpp"
#include "VulkanRendering/Buffers.hpp"

struct MemoryLocation
{
	VkDeviceSize startOffset;
	VkDeviceSize endOffset;

	MemoryLocation(VkDeviceSize startOffset, VkDeviceSize endOffset) : startOffset(startOffset), endOffset(endOffset) {}
	VkDeviceSize getSize()
	{
		return endOffset - startOffset + 1;
	}
};

class GPUMemoryBlock
{
public:
	VkDeviceSize blockSize;
	VkBuffer buffer;
	VkDeviceMemory memory;

	uint32_t maxSingleDrawCallIndexCount;

	std::vector<MemoryLocation> usedDataLocations;
	std::vector<MemoryLocation> emptyDataLocations;

	GPUMemoryBlock() {}

	GPUMemoryBlock(VulkanCoreInfo* vulkanCoreInfo, VkDeviceSize size, uint32_t maxSingleDrawCallIndexAmount)
	{
		if (size - 1 > UINT32_MAX) {
			throw std::runtime_error("GPUMemoryBlock is too large!");
		}

		this->maxSingleDrawCallIndexCount = maxSingleDrawCallIndexAmount;

		blockSize = size;
		createBuffer(vulkanCoreInfo, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory);
		emptyDataLocations.push_back(MemoryLocation(0, size - 1));
	}

	void addVertices(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices);
	void trackAddedMemoryLocation(VkDeviceSize memoryOffsetStart, VkDeviceSize size);
	void getVerticesToRender(
		VkBuffer& vertexBuffer,
		std::vector<VkDeviceSize>& vertexOffsets,
		std::vector<uint32_t>& batchIndexCounts);
	void getVerticesToRender2(
		VkDeviceSize maxVertexInputBindings,
		std::vector<uint32_t>& batchBindingCounts,
		std::vector<VkBuffer*>& batchVertexBuffers,
		std::vector<VkDeviceSize*>& batchVertexOffsets,
		std::vector<VkDeviceSize*>& batchVertexSizes,
		std::vector<VkDeviceSize*>& batchVertexStrides,
		std::vector<uint32_t>& batchIndexCounts);
	void debugPrint();
	uint32_t getVertexCount();
	void cleanup(VulkanCoreInfo* vulkanCoreInfo);

private:
	bool findBestVertexLocation(VkDeviceSize size, VkDeviceSize& location);
	void copyVerticesToLocation(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkDeviceSize memoryOffsetStart, VkDeviceSize size, std::vector<Vertex>& vertices);
};