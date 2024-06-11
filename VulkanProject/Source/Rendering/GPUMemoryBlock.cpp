#include "GPUMemoryBlock.hpp"

#include <iostream>

void GPUMemoryBlock::addVertices(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices)
{
    VkDeviceSize verticesSize = sizeof(vertices[0]) * vertices.size();
    VkDeviceSize memoryOffsetStart;
    bool foundPlace = findBestVertexLocation(verticesSize, memoryOffsetStart);
    if (!foundPlace) {
        std::cout << "Could not find space for vertex data in memory block\n\n";
        debugPrint();
        return;
    }
    copyVerticesToLocation(vulkanCoreInfo, commandPool, memoryOffsetStart, verticesSize, vertices);
    trackAddedMemoryLocation(memoryOffsetStart, verticesSize);
}

bool GPUMemoryBlock::findBestVertexLocation(VkDeviceSize size, VkDeviceSize& location)
{
    for (int i = 0; i < emptyDataLocations.size(); i++) {
        if (emptyDataLocations[i].getSize() > size) {
            location = emptyDataLocations[i].startOffset;
            return true;
        }
    }
    return false;
}

void GPUMemoryBlock::copyVerticesToLocation(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkDeviceSize memoryOffsetStart, VkDeviceSize size, std::vector<Vertex>& vertices)
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

void GPUMemoryBlock::trackAddedMemoryLocation(VkDeviceSize memoryOffsetStart, VkDeviceSize size)
{
    VkDeviceSize memoryOffsetEnd = memoryOffsetStart + size - 1;

    for (int i = 0; i < emptyDataLocations.size(); i++) {
        if (memoryOffsetStart >= emptyDataLocations[i].startOffset && memoryOffsetEnd <= emptyDataLocations[i].endOffset) {

            // are gaps left at the front of the allocation
            bool gapsInFront = emptyDataLocations[i].startOffset != memoryOffsetStart;
            // are gaps left at the end of the allocation
            bool gapsInEnd = emptyDataLocations[i].endOffset != memoryOffsetEnd;

            usedDataLocations.push_back(MemoryLocation(memoryOffsetStart, memoryOffsetEnd));
            //std::cout << "gaps in front = " << gapsInFront << " gaps in end = " << gapsInEnd << '\n';

            if (gapsInFront && gapsInEnd) {
                emptyDataLocations.push_back(MemoryLocation(memoryOffsetEnd + 1, emptyDataLocations[i].endOffset));
                emptyDataLocations[i].endOffset = memoryOffsetStart - 1;
            }
            else if (gapsInFront && !gapsInEnd) {
                emptyDataLocations[i].endOffset = memoryOffsetStart - 1;
            }
            else if (!gapsInFront && gapsInEnd) {
                emptyDataLocations[i].startOffset = memoryOffsetEnd + 1;
            }
            else {
                emptyDataLocations.erase(emptyDataLocations.begin() + i);
            }
        }
    }
    //debugPrint();
}

void GPUMemoryBlock::getVerticesToRender(
    VkBuffer& vertexBuffer,
    std::vector<VkDeviceSize>& vertexOffsets,
    std::vector<uint32_t>& batchIndexCounts)
{
    vertexBuffer = buffer;

    for (int i = 0; i < usedDataLocations.size(); i++) {

        uint32_t indexCount = usedDataLocations[i].getSize() / sizeof(Vertex) * 1.5f; // get vertex count and multiply with indices/vertices = 1.5f.
        bool isConsecutiveData = i != 0 && usedDataLocations[i - 1].endOffset + 1 == usedDataLocations[i].startOffset;

        if (isConsecutiveData && batchIndexCounts.back() + indexCount <= maxSingleDrawCallIndexCount) {

            batchIndexCounts.back() += indexCount;
        }
        else {
            vertexOffsets.push_back(usedDataLocations[i].startOffset);
            batchIndexCounts.push_back(indexCount);
        }
    }
}

void GPUMemoryBlock::getVerticesToRender2(
    VkDeviceSize maxVertexInputBindings,
    std::vector<uint32_t>& batchBindingCounts,
    std::vector<VkBuffer*>& batchVertexBuffers,
    std::vector<VkDeviceSize*>& batchVertexOffsets,
    std::vector<VkDeviceSize*>& batchVertexSizes,
    std::vector<VkDeviceSize*>& batchVertexStrides,
    std::vector<uint32_t>& batchIndexCounts)
{
    uint32_t batchCount = (uint32_t)(std::ceil(((float)usedDataLocations.size()) / ((float)maxVertexInputBindings)));

    //std::cout << "\n" << "batch count = " << batchCount << "\n";

    uint32_t vertexBufferCounter = 0;
    
    for (int i = 0; i < batchCount; i++) {
        uint32_t batchBufferCount = 0;
        if (i == batchCount - 1) {
            batchBufferCount = usedDataLocations.size() % maxVertexInputBindings;
        }
        else {
            batchBufferCount = maxVertexInputBindings;
        }
        batchBindingCounts.push_back(batchBufferCount);
        
        batchVertexBuffers.push_back(new VkBuffer[batchBindingCounts[i]]);
        batchVertexOffsets.push_back(new VkDeviceSize[batchBindingCounts[i]]);
        batchVertexSizes.push_back(new VkDeviceSize[batchBindingCounts[i]]);
        batchVertexStrides.push_back(new VkDeviceSize[batchBindingCounts[i]]);

        uint32_t batchVertexCount = 0;

        for (int k = 0; k < batchBufferCount; k++) {
            batchVertexBuffers[i][k] = buffer;
            batchVertexOffsets[i][k] = usedDataLocations[vertexBufferCounter].startOffset;
            //std::cout << "start offset = " << usedDataLocations[vertexBufferCounter].startOffset << "end offset = " << usedDataLocations[vertexBufferCounter].endOffset << "\n";

            uint32_t verticesSize = usedDataLocations[vertexBufferCounter].getSize();
            //std::cout << "vertex count = " << usedDataLocations[vertexBufferCounter].getSize() / sizeof(Vertex) << "\n";
            batchVertexSizes[i][k] = verticesSize;
            batchVertexCount += verticesSize / sizeof(Vertex);

            batchVertexStrides[i][k] = sizeof(Vertex);

            vertexBufferCounter++;
        }
        // there are 6 indices per 4 vertices.
        batchIndexCounts.push_back((uint32_t)(batchVertexCount * 1.5f));
        //std::cout << "index count = " << (uint32_t)(batchVertexCount * 1.5f) << "\n";
    }
}

void GPUMemoryBlock::debugPrint()
{
    for (int i = 0; i < emptyDataLocations.size(); i++) {
        std::cout << "empty data: " << emptyDataLocations[i].startOffset << " to " << emptyDataLocations[i].endOffset << '\n';
    }
    for (int i = 0; i < usedDataLocations.size(); i++) {
        std::cout << "used data: " << usedDataLocations[i].startOffset << " to " << usedDataLocations[i].endOffset << '\n';
    }
}

uint32_t GPUMemoryBlock::getVertexCount()
{
    uint32_t vertexCount = 0;

    for (int i = 0; i < usedDataLocations.size(); i++) {
        vertexCount += usedDataLocations[i].getSize() / sizeof(Vertex);
    }
    return vertexCount;
}

void GPUMemoryBlock::cleanup(VulkanCoreInfo* vulkanCoreInfo)
{
    vkDestroyBuffer(vulkanCoreInfo->device, buffer, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, memory, nullptr);
}