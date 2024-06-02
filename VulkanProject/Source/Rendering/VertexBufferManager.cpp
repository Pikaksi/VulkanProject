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

void VertexBufferManager::createVertexBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer& buffer, VkDeviceMemory& memory, std::vector<Vertex>& vertices) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(vulkanCoreInfo, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vulkanCoreInfo->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(vulkanCoreInfo->device, stagingBufferMemory);

    createBuffer(vulkanCoreInfo, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory);

    copyBuffer(vulkanCoreInfo, commandPool, stagingBuffer, buffer, bufferSize);

    vkDestroyBuffer(vulkanCoreInfo->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, stagingBufferMemory, nullptr);
}

void VertexBufferManager::createIndexBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer& buffer, VkDeviceMemory& memory, std::vector<uint32_t>& indices) {

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(vulkanCoreInfo, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vulkanCoreInfo->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(vulkanCoreInfo->device, stagingBufferMemory);

    createBuffer(vulkanCoreInfo, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory);
    //auto startTime = std::chrono::high_resolution_clock::now();

    copyBuffer(vulkanCoreInfo, commandPool, stagingBuffer, buffer, bufferSize);

    //auto endTime = std::chrono::high_resolution_clock::now();
    //float timePassed = std::chrono::duration<float, std::chrono::seconds::period>(endTime - startTime).count();
    //std::cout << "Creating index buffer took " << timePassed << " seconds.\n";
    vkDestroyBuffer(vulkanCoreInfo->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, stagingBufferMemory, nullptr);
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