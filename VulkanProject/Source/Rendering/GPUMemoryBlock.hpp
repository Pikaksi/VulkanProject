#pragma once

#include "vulkan/vulkan.h"

#include <vector>
#include <unordered_map>
#include <stdexcept>

#include "VulkanRendering/VulkanTypes.hpp"
#include "VulkanRendering/Buffers.hpp"

struct MemoryLocation
{
	uint32_t startOffset;
	uint32_t endOffset;

	MemoryLocation(uint32_t startOffset, uint32_t endOffset) : startOffset(startOffset), endOffset(endOffset) {}
	uint32_t getSize()
	{
		return endOffset - startOffset + 1;
	}
};

// has dictionaries to look up the starts and ends of used and unused memory locations
class GPUMemoryBlock
{
public:
	GPUMemoryBlock() {}
	GPUMemoryBlock(VulkanCoreInfo* vulkanCoreInfo, VkDeviceSize size, uint32_t maxSingleDrawCallIndexAmount);

	bool addVertices(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, uint32_t& memoryLocation, std::vector<Vertex>& vertices);
	void freeMemory(uint32_t& allocationStartOffset);
	void getVerticesToRender(
		VkBuffer& vertexBuffer,
		std::vector<VkDeviceSize>& vertexOffsets,
		std::vector<uint32_t>& batchIndexCounts);
	void cleanup(VulkanCoreInfo* vulkanCoreInfo);

	void debugPrint();
	uint32_t getVertexCount();

	VkBuffer buffer;

private:
	uint32_t blockSize;
	VkDeviceMemory memory;

	uint32_t maxSingleDrawCallIndexCount;

	std::unordered_map<uint32_t, MemoryLocation> usedMemoryLocationsStartLookup;
	std::unordered_map<uint32_t, MemoryLocation> usedMemoryLocationsEndLookup;
	std::unordered_map<uint32_t, MemoryLocation> emptyMemoryLocationsStartLookup;
	std::unordered_map<uint32_t, MemoryLocation> emptyMemoryLocationsEndLookup;


	bool allocateMemory(uint32_t size, uint32_t& location);
	void trackAddedMemoryLocation(uint32_t allocationStartOffset, uint32_t size);
	void copyVerticesToLocation(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, uint32_t memoryOffsetStart, uint32_t size, std::vector<Vertex>& vertices);
};