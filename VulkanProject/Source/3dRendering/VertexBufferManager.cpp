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
    worldGPUMemoryBlock = GPUMemoryBlock(vulkanCoreInfo, sizeof(Vertex) * 40000000, quadStripIndexBufferIndexCount);
    /*std::vector<Vertex> vertices;
    vertices.push_back(Vertex{ {0, 0, 0}, {1, 1, 1}, {0, 0} });
    vertices.push_back(Vertex{ {100, 0, 0}, {1, 0, 1}, {0, 1} });
    vertices.push_back(Vertex{ {0, 100, 0}, {1, 1, 0}, {1, 0} });
    vertices.push_back(Vertex{ {100, 100, 0}, {0, 0, 1}, {1, 1} });
    vertices.push_back(Vertex{ {0, 0, 10}, {1, 1, 1}, {0, 0} });
    vertices.push_back(Vertex{ {100, 0, 10}, {1, 0, 1}, {0, 1} });
    vertices.push_back(Vertex{ {0, 100, 10}, {0, 0, 0}, {1, 0} });
    vertices.push_back(Vertex{ {100, 100, 10}, {0, 0, 1}, {1, 1} });
    worldGPUMemoryBlock.addVertices(vulkanCoreInfo, commandPool, vertices);
    for (int i = 0; i < 16; i++) {
        std::vector<Vertex> vertices2;
        vertices2.push_back(Vertex{ {0, 0, 30}, {1, 1, 1}, {0, 0} });
        vertices2.push_back(Vertex{ {100, 0, 30}, {1, 0, 1}, {0, 1} });
        vertices2.push_back(Vertex{ {0, 100, 30}, {0, 0, 0}, {1, 0} });
        vertices2.push_back(Vertex{ {100, 100, 30}, {0, 0, 1}, {1, 1} });
        worldGPUMemoryBlock.addVertices(vulkanCoreInfo, commandPool, vertices2);

    }
    std::cout << "------------------------------------------------------\n";
    worldGPUMemoryBlock.debugPrint();
    std::cout << "------------------------------------------------------\n";
    gPUMemoryBlock.trackAddedMemoryLocation(0, 2);
    std::cout << "------------------------------------------------------\n";
    gPUMemoryBlock.trackAddedMemoryLocation(5, 1);
    std::cout << "------------------------------------------------------\n";
    gPUMemoryBlock.trackAddedMemoryLocation(8, 2);
    std::cout << "------------------------------------------------------\n";
    gPUMemoryBlock.trackAddedMemoryLocation(1, 2);
    std::cout << "------------------------------------------------------\n";*/
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