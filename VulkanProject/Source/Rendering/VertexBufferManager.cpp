#include "VertexBufferManager.hpp"

void VertexBufferManager::addChunkVertices(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    VkBuffer vertexBuffer, indexBuffer;
    VkDeviceMemory vertexBufferMemory, indexBufferMemory;

    //std::cout << "vertex buffer size = " << vertices.size() << "\n";

    createVertexBuffer(vertexBuffer, vertexBufferMemory, vertices);
    createIndexBuffer(indexBuffer, indexBufferMemory, indices);

    vertexCounts.push_back(vertices.size());
    vertexBuffers.push_back(vertexBuffer);
    vertexBufferMemories.push_back(vertexBufferMemory);

    indexCounts.push_back(indices.size());
    indexBuffers.push_back(indexBuffer);
    indexBufferMemories.push_back(indexBufferMemory);
}

void VertexBufferManager::createVertexBuffer(VkBuffer& buffer, VkDeviceMemory& memory, std::vector<Vertex>& vertices) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Application::getInstance().createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(Application::getInstance().device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(Application::getInstance().device, stagingBufferMemory);

    Application::getInstance().createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory);

    Application::getInstance().copyBuffer(stagingBuffer, buffer, bufferSize);

    vkDestroyBuffer(Application::getInstance().device, stagingBuffer, nullptr);
    vkFreeMemory(Application::getInstance().device, stagingBufferMemory, nullptr);
}

void VertexBufferManager::createIndexBuffer(VkBuffer& buffer, VkDeviceMemory& memory, std::vector<uint32_t>& indices) {

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Application::getInstance().createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(Application::getInstance().device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(Application::getInstance().device, stagingBufferMemory);

    Application::getInstance().createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory);
    //auto startTime = std::chrono::high_resolution_clock::now();

    Application::getInstance().copyBuffer(stagingBuffer, buffer, bufferSize);

    //auto endTime = std::chrono::high_resolution_clock::now();
    //float timePassed = std::chrono::duration<float, std::chrono::seconds::period>(endTime - startTime).count();
    //std::cout << "Creating index buffer took " << timePassed << " seconds.\n";
    vkDestroyBuffer(Application::getInstance().device, stagingBuffer, nullptr);
    vkFreeMemory(Application::getInstance().device, stagingBufferMemory, nullptr);
}

void VertexBufferManager::cleanUpBuffers()
{
    for (VkBuffer vertexBuffer : vertexBuffers) {
        vkDestroyBuffer(Application::getInstance().device, vertexBuffer, nullptr);
    }
    for (VkDeviceMemory vertexBufferMemory : vertexBufferMemories) {
        vkFreeMemory(Application::getInstance().device, vertexBufferMemory, nullptr);
    }

    for (VkBuffer indexBuffer : indexBuffers) {
        vkDestroyBuffer(Application::getInstance().device, indexBuffer, nullptr);
    }
    for (VkDeviceMemory indexBufferMemory : indexBufferMemories) {
        vkFreeMemory(Application::getInstance().device, indexBufferMemory, nullptr);
    }
}