#include "VertexBufferManager.hpp"

void VertexBufferManager::addChunkVertices(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    VkBuffer vertexBuffer, indexBuffer;
    VkDeviceMemory vertexBufferMemory, indexBufferMemory;

    //std::cout << "vertex buffer size = " << vertices.size() << "\n";

    createVertexBuffer(vulkanCoreInfo, commandPool, vertexBuffer, vertexBufferMemory, vertices);
    createIndexBuffer(vulkanCoreInfo, commandPool, indexBuffer, indexBufferMemory, indices);

    vertexCounts.push_back(vertices.size());
    vertexBuffers.push_back(vertexBuffer);
    vertexBufferMemories.push_back(vertexBufferMemory);

    indexCounts.push_back(indices.size());
    indexBuffers.push_back(indexBuffer);
    indexBufferMemories.push_back(indexBufferMemory);
}

void VertexBufferManager::cleanUpBuffers(VulkanCoreInfo* vulkanCoreInfo)
{
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