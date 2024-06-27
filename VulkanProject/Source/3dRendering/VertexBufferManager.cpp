#include "VertexBufferManager.hpp"

uint32_t VertexBufferManager::addChunkVertices(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    uint32_t memoryLocation;
    worldGPUMemoryBlock.addVertices(vulkanCoreInfo, commandPool, memoryLocation, vertices);
    return memoryLocation;
}

void VertexBufferManager::fillLargeQuadStripIndexBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool)
{
    std::vector<uint32_t> indices;
    for (int i = 0; i < quadStripIndexBufferQuadAmount; i++) {
        indices.push_back(i * 4);
        indices.push_back(i * 4 + 1);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 3);
        indices.push_back(i * 4);
    }

    createIndexBuffer(vulkanCoreInfo, commandPool, quadStripIndexBuffer, quadStripIndexBufferMemory, indices);
}

void VertexBufferManager::derenderChunk(uint32_t memoryBlockLocation)
{
    /*std::cout << "---------------------------\n";
    worldGPUMemoryBlock.debugPrint();
    std::cout << "---------------------------\n";*/
    worldGPUMemoryBlock.freeMemory(memoryBlockLocation);
    /*std::cout << "---------------------------\n";
    worldGPUMemoryBlock.debugPrint();
    std::cout << "---------------------------\n";*/
}

void VertexBufferManager::createGPUMemoryBlocks(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool)
{
    worldGPUMemoryBlock = GPUMemoryBlock(vulkanCoreInfo, sizeof(Vertex) * 100000, quadStripIndexBufferIndexCount);
}

void VertexBufferManager::cleanUpBuffers(VulkanCoreInfo* vulkanCoreInfo)
{
    worldGPUMemoryBlock.cleanup(vulkanCoreInfo);

    vkDestroyBuffer(vulkanCoreInfo->device, quadStripIndexBuffer, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, quadStripIndexBufferMemory, nullptr);

    for (VkBuffer vertexBuffer : vertexBuffers) {
        vkDestroyBuffer(vulkanCoreInfo->device, vertexBuffer, nullptr);
    }
    for (VkDeviceMemory vertexBufferMemory : vertexBufferMemories) {
        vkFreeMemory(vulkanCoreInfo->device, vertexBufferMemory, nullptr);
    }

    for (VkBuffer indexBuffer : indexBuffers) {
        vkDestroyBuffer(vulkanCoreInfo->device, indexBuffer, nullptr);
    }
    for (VkDeviceMemory indexBufferMemory : indexBufferMemories) {
        vkFreeMemory(vulkanCoreInfo->device, indexBufferMemory, nullptr);
    }
}