#include "GPUMemoryBlock.hpp"

#include <iostream>

GPUMemoryBlock::GPUMemoryBlock(VulkanCoreInfo& vulkanCoreInfo, uint32_t sizeofData, uint32_t allocationDataCount, uint32_t mergedDataMaxCount)
    : blockSize(sizeofData * allocationDataCount), sizeofData(sizeofData), mergedDataMaxCount(mergedDataMaxCount)
{
    if (blockSize >= UINT32_MAX) {
        throw std::runtime_error("GPUMemoryBlock is too large!");
    }

    createBuffer(vulkanCoreInfo, blockSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory);
    emptyMemoryLocationsStartLookup.insert(std::make_pair(0, MemoryLocation(0, blockSize - 1)));
    emptyMemoryLocationsEndLookup.insert(std::make_pair(blockSize - 1, MemoryLocation(0, blockSize - 1)));
}

// returns true if successful
bool GPUMemoryBlock::allocateMemory(uint32_t size, uint32_t& location)
{
    for (const auto pair : emptyMemoryLocationsStartLookup) {
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

void GPUMemoryBlock::getVertexDataMerged(VkBuffer& outVertexBuffer, std::vector<VkDeviceSize>& vertexOffsets, std::vector<uint32_t>& batchVertexCounts)
{
    outVertexBuffer = buffer;

    uint32_t iteratorLocation = 0;
    bool gapsBetweenLastMemory = true;

    while (iteratorLocation < blockSize - 1) {

        if (usedMemoryLocationsStartLookup.contains(iteratorLocation)) {
            MemoryLocation currentMemoryBlock = usedMemoryLocationsStartLookup.at(iteratorLocation);
            uint32_t dataCount = currentMemoryBlock.getSize() / sizeofData;

            if (gapsBetweenLastMemory || batchVertexCounts.back() + dataCount > mergedDataMaxCount) {
                vertexOffsets.push_back(iteratorLocation);
                batchVertexCounts.push_back(dataCount);
            }
            else {
                batchVertexCounts.back() += dataCount;
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

void GPUMemoryBlock::getVertexData(VkBuffer& outVertexBuffer, std::vector<VkDeviceSize>& vertexOffsets, std::vector<uint32_t>& batchVertexCounts)
{
    outVertexBuffer = buffer;

    for (auto pair : usedMemoryLocationsStartLookup) {
        vertexOffsets.push_back(pair.first);

        uint32_t dataCount = pair.second.getSize() / sizeofData;
        batchVertexCounts.push_back(dataCount);
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

uint32_t GPUMemoryBlock::getDataCount()
{
    uint32_t dataCount = 0;

    for (auto pair : usedMemoryLocationsStartLookup) {
        dataCount += pair.second.getSize();
    }
    return dataCount / sizeofData;
}

void GPUMemoryBlock::cleanup(VulkanCoreInfo& vulkanCoreInfo)
{
    vkDestroyBuffer(vulkanCoreInfo.device, buffer, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, memory, nullptr);
}