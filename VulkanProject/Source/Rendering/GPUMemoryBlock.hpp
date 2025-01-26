#pragma once

#include "vulkan/vulkan.h"

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

#include "VulkanRendering/VulkanTypes.hpp"
#include "VulkanRendering/Buffers.hpp"

struct MemoryLocation
{
	uint32_t startOffset;
	uint32_t endOffset;

	MemoryLocation(uint32_t startOffset, uint32_t endOffset) : startOffset(startOffset), endOffset(endOffset) {}
	uint32_t getSize() const
	{
		return endOffset - startOffset + 1;
	}
};

// Buffer uses VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
class GPUMemoryBlock
{
public:
	void freeMemory(uint32_t& allocationStartOffset);
	void getVertexDataMerged(VkBuffer& buffer, std::vector<VkDeviceSize>& offsets, std::vector<uint32_t>& batchCounts);
	void getVertexData(VkBuffer& outVertexBuffer, std::vector<VkDeviceSize>& vertexOffsets, std::vector<uint32_t>& batchVertexCounts);
	void cleanup(VulkanCoreInfo& vulkanCoreInfo);

	GPUMemoryBlock() {}
	GPUMemoryBlock(VulkanCoreInfo& vulkanCoreInfo, uint32_t sizeofData, uint32_t allocationDataCount, uint32_t mergedDataMaxCount);

	void debugPrint();
	uint32_t getDataCount();

	VkBuffer buffer;

	template<typename T>
	void addData(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, uint32_t& memoryLocation, std::vector<T>& data)
	{
		uint32_t dataSize = sizeof(T) * data.size();

		if (!allocateMemory(dataSize, memoryLocation)) {
			throw std::runtime_error ("Could not find space for data in memory block\n");
		}
		copyDataToLocation(vulkanCoreInfo, commandPool, memoryLocation, dataSize, data);
	}

private:
	template<typename T>
	void copyDataToLocation(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, uint32_t memoryOffsetStart, uint32_t size, std::vector<T>& data)
	{
		if (memoryOffsetStart + size > blockSize) {
			std::cout << "tried to write data at " << memoryOffsetStart << " to " << memoryOffsetStart + size << " block size is " << blockSize << '\n';
			throw std::runtime_error("Data write is out of memory block!\n");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(vulkanCoreInfo, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* stagingBufferPointer;
		vkMapMemory(vulkanCoreInfo.device, stagingBufferMemory, 0, size, 0, &stagingBufferPointer);
		memcpy(stagingBufferPointer, data.data(), (size_t)size);
		vkUnmapMemory(vulkanCoreInfo.device, stagingBufferMemory);

		copyBuffer(vulkanCoreInfo, commandPool, stagingBuffer, buffer, size, 0, memoryOffsetStart);

		vkDestroyBuffer(vulkanCoreInfo.device, stagingBuffer, nullptr);
		vkFreeMemory(vulkanCoreInfo.device, stagingBufferMemory, nullptr);
	}

	uint32_t sizeofData;
	uint32_t blockSize;
	VkDeviceMemory memory;

	uint32_t mergedDataMaxCount;

	std::unordered_map<uint32_t, MemoryLocation> usedMemoryLocationsStartLookup;
	std::unordered_map<uint32_t, MemoryLocation> usedMemoryLocationsEndLookup;
	std::unordered_map<uint32_t, MemoryLocation> emptyMemoryLocationsStartLookup;
	std::unordered_map<uint32_t, MemoryLocation> emptyMemoryLocationsEndLookup;


	bool allocateMemory(uint32_t size, uint32_t& location);
	void trackAddedMemoryLocation(uint32_t allocationStartOffset, uint32_t size);
};