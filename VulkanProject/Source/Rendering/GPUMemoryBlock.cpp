#include "GPUMemoryBlock.hpp"

#include <iostream>

GPUMemoryBlock::GPUMemoryBlock(VulkanCoreInfo* vulkanCoreInfo, VkDeviceSize size, uint32_t maxSingleDrawCallIndexAmount)
{
    if (size >= UINT32_MAX) {
        throw std::runtime_error("GPUMemoryBlock is too large!");
    }

    this->maxSingleDrawCallIndexCount = maxSingleDrawCallIndexAmount;

    blockSize = size;
    createBuffer(vulkanCoreInfo, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory);
    emptyMemoryLocationsStartLookup.insert(std::make_pair(0, MemoryLocation(0, size - 1)));
    emptyMemoryLocationsEndLookup.insert(std::make_pair(size - 1, MemoryLocation(0, size - 1)));
}

bool GPUMemoryBlock::addVertices(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, uint32_t& memoryLocation, std::vector<Vertex>& vertices)
{
    uint32_t verticesSize = sizeof(vertices[0]) * vertices.size();
    //std::cout << "added chunk with vertices size: " << verticesSize << "\n";

    if (!allocateMemory(verticesSize, memoryLocation)) {
        std::cout << "Could not find space for vertex data in memory block\n";
        return false;
    }
    copyVerticesToLocation(vulkanCoreInfo, commandPool, memoryLocation, verticesSize, vertices);
    return true;
}

// returns true if location was found
bool GPUMemoryBlock::allocateMemory(uint32_t size, uint32_t& location)
{
    for (auto pair : emptyMemoryLocationsStartLookup) {
        if (pair.second.getSize() > size) {

            trackAddedMemoryLocation(pair.second.startOffset, size);
            location = pair.second.startOffset;

            return true;
        }
    }
    return false;
}

void GPUMemoryBlock::freeMemory(uint32_t& allocationStartOffset)
{
    if (!usedMemoryLocationsStartLookup.contains(allocationStartOffset)) {
        throw std::runtime_error("tried to delete memory when it was not allocated!");
    }

    uint32_t allocationEndOffset = usedMemoryLocationsStartLookup.at(allocationStartOffset).endOffset;

    // Is there empty space before the deallocation
    bool emptyBefore = emptyMemoryLocationsEndLookup.contains(allocationStartOffset - 1);
    // Is there empty space after the deallocation
    bool emptyAfter = emptyMemoryLocationsStartLookup.contains(allocationEndOffset + 1);

    usedMemoryLocationsStartLookup.erase(allocationStartOffset);
    usedMemoryLocationsEndLookup.erase(allocationEndOffset);

    if (emptyBefore && emptyAfter) {
        uint32_t emptyBeforeStartOffset = emptyMemoryLocationsEndLookup.at(allocationStartOffset - 1).startOffset;
        uint32_t emptyAfterEndOffset = emptyMemoryLocationsStartLookup.at(allocationEndOffset + 1).endOffset;

        emptyMemoryLocationsEndLookup.erase(emptyAfterEndOffset);
        emptyMemoryLocationsStartLookup.erase(allocationEndOffset + 1);

        emptyMemoryLocationsEndLookup.erase(allocationStartOffset - 1);
        emptyMemoryLocationsStartLookup.erase(emptyBeforeStartOffset);

        emptyMemoryLocationsStartLookup.insert(std::make_pair(emptyBeforeStartOffset, MemoryLocation(emptyBeforeStartOffset, emptyAfterEndOffset)));
        emptyMemoryLocationsEndLookup.insert(std::make_pair(emptyAfterEndOffset, MemoryLocation(emptyBeforeStartOffset, emptyAfterEndOffset)));
    }
    else if (emptyBefore) {
        uint32_t emptyBeforeStartOffset = emptyMemoryLocationsEndLookup.at(allocationStartOffset - 1).startOffset;

        emptyMemoryLocationsEndLookup.erase(allocationStartOffset - 1);
        emptyMemoryLocationsStartLookup.erase(emptyBeforeStartOffset);

        emptyMemoryLocationsStartLookup.insert(std::make_pair(emptyBeforeStartOffset, MemoryLocation(emptyBeforeStartOffset, allocationEndOffset)));
        emptyMemoryLocationsEndLookup.insert(std::make_pair(allocationEndOffset, MemoryLocation(emptyBeforeStartOffset, allocationEndOffset)));
    }
    else if (emptyAfter) {
        uint32_t emptyAfterEndOffset = emptyMemoryLocationsStartLookup.at(allocationEndOffset + 1).endOffset;

        emptyMemoryLocationsEndLookup.erase(emptyAfterEndOffset);
        emptyMemoryLocationsStartLookup.erase(allocationEndOffset + 1);

        emptyMemoryLocationsStartLookup.insert(std::make_pair(allocationStartOffset, MemoryLocation(allocationStartOffset, emptyAfterEndOffset)));
        emptyMemoryLocationsEndLookup.insert(std::make_pair(emptyAfterEndOffset, MemoryLocation(allocationStartOffset, emptyAfterEndOffset)));
    }
    else {
        emptyMemoryLocationsStartLookup.insert(std::make_pair(allocationStartOffset, MemoryLocation(allocationStartOffset, allocationEndOffset)));
        emptyMemoryLocationsEndLookup.insert(std::make_pair(allocationEndOffset, MemoryLocation(allocationStartOffset, allocationEndOffset)));
    }
}

void GPUMemoryBlock::copyVerticesToLocation(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, uint32_t memoryOffsetStart, uint32_t size, std::vector<Vertex>& vertices)
{
    if (memoryOffsetStart + size > blockSize) {
        std::cout << "tried to write data at " << memoryOffsetStart << " to " << memoryOffsetStart + size << " block size is " << blockSize << '\n';
        throw std::runtime_error("Vertex data write is out of memory block!\n");
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(vulkanCoreInfo, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vulkanCoreInfo->device, stagingBufferMemory, 0, size, 0, &data);
    memcpy(data, vertices.data(), (size_t)size);
    vkUnmapMemory(vulkanCoreInfo->device, stagingBufferMemory);

    copyBuffer(vulkanCoreInfo, commandPool, stagingBuffer, buffer, size, 0, memoryOffsetStart);

    vkDestroyBuffer(vulkanCoreInfo->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, stagingBufferMemory, nullptr);
}

void GPUMemoryBlock::trackAddedMemoryLocation(uint32_t allocationStartOffset, uint32_t size)
{
    uint32_t allocationEndOffset = allocationStartOffset + size - 1;

    MemoryLocation allocatedMemoryLocation = MemoryLocation(allocationStartOffset, allocationEndOffset);
    usedMemoryLocationsStartLookup.insert(std::make_pair(allocationStartOffset, allocatedMemoryLocation));
    usedMemoryLocationsEndLookup.insert(std::make_pair(allocationEndOffset, allocatedMemoryLocation));

    // Are gaps left at the end of the allocation.
    bool gapsInEnd = !emptyMemoryLocationsStartLookup.contains(allocationEndOffset + 1);

    // the allocator should always find a location such that there is no empty space before the allocation.
    if (gapsInEnd) {
        uint32_t emptyRegionEndOffset = emptyMemoryLocationsStartLookup.at(allocationStartOffset).endOffset;

        emptyMemoryLocationsStartLookup.erase(allocationStartOffset);
        emptyMemoryLocationsEndLookup.erase(emptyRegionEndOffset);

        MemoryLocation emptyMemoryLocation = MemoryLocation(allocationEndOffset + 1, emptyRegionEndOffset);
        emptyMemoryLocationsStartLookup.insert(std::make_pair(allocationEndOffset + 1, emptyMemoryLocation));
        emptyMemoryLocationsEndLookup.insert(std::make_pair(emptyRegionEndOffset, emptyMemoryLocation));
    }
    else {
        emptyMemoryLocationsStartLookup.erase(allocationStartOffset);
        emptyMemoryLocationsEndLookup.erase(allocationEndOffset);
    }
}

void GPUMemoryBlock::getVerticesToRender(
    VkBuffer& vertexBuffer,
    std::vector<VkDeviceSize>& vertexOffsets,
    std::vector<uint32_t>& batchIndexCounts)
{
    vertexBuffer = buffer;

    uint32_t iteratorLocation = 0;
    bool gapsBetweenLastMemory = true;

    while (iteratorLocation < blockSize - 1) {

        if (usedMemoryLocationsStartLookup.contains(iteratorLocation)) {
            MemoryLocation currentMemoryBlock = usedMemoryLocationsStartLookup.at(iteratorLocation);
            // get index count by getting vertex count and multiply with indices/vertices = 1.5f.
            uint32_t indexCount = currentMemoryBlock.getSize() / sizeof(Vertex) * 1.5f;

            if (gapsBetweenLastMemory || batchIndexCounts.back() + indexCount > maxSingleDrawCallIndexCount) {
                vertexOffsets.push_back(iteratorLocation);
                batchIndexCounts.push_back(indexCount);
            }
            else {
                batchIndexCounts.back() += indexCount;
            }
            iteratorLocation = currentMemoryBlock.endOffset + 1;
            gapsBetweenLastMemory = false;
        }
        else {
            iteratorLocation = emptyMemoryLocationsStartLookup.at(iteratorLocation).endOffset + 1;
            gapsBetweenLastMemory = true;
        }
    }
}

void GPUMemoryBlock::debugPrint()
{
    for (auto pair : emptyMemoryLocationsStartLookup) {
        std::cout << "empty data: " << pair.second.startOffset << " to " << pair.second.endOffset << " size: " << pair.second.getSize() << '\n';
    }
    for (auto pair : usedMemoryLocationsStartLookup) {
        std::cout << "used data: " << pair.second.startOffset << " to " << pair.second.endOffset << " size: " << pair.second.getSize() << '\n';
    }
}

uint32_t GPUMemoryBlock::getVertexCount()
{
    uint32_t vertexCount = 0;

    for (auto pair : usedMemoryLocationsStartLookup) {
        vertexCount += pair.second.getSize();
    }
    return vertexCount / sizeof(Vertex);
}

void GPUMemoryBlock::cleanup(VulkanCoreInfo* vulkanCoreInfo)
{
    vkDestroyBuffer(vulkanCoreInfo->device, buffer, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, memory, nullptr);
}