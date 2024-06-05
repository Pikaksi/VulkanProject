#include "UIManager.hpp"

#include <iostream>

void UIManager::refreshUI(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, int screenWidth, int screenHeight)
{
    cleanUpBuffers(vulkanCoreInfo);

    std::vector<Vertex> newVertices;
    std::vector<uint32_t> newIndices;

    for (int i = 0; i < currentUIElements.size(); i++) {
        currentUIElements[i].addMeshData(screenWidth, screenHeight, newVertices, newIndices);
    }

    createVertexBuffer(vulkanCoreInfo, commandPool, vertexBuffer, vertexBufferMemory, newVertices);
    createIndexBuffer(vulkanCoreInfo, commandPool, indexBuffer, indexBufferMemory, newIndices);
    indexCount = newIndices.size();
    /*VkBuffer test1; VkDeviceMemory test2;
    createVertexBuffer(vulkanCoreInfo, commandPool, test1, test2, newVertices);
    vertexBuffer.push_back(test1);
    vertexBufferMemory.push_back(test2);

    VkBuffer test3; VkDeviceMemory test4;
    createIndexBuffer(vulkanCoreInfo, commandPool, test3, test4, newIndices);
    indexBuffer.push_back(test3);
    std::cout << "addedwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww " << indexBuffer.size() << "\n";
    indexBufferMemory.push_back(test4);
    indexCount.push_back(newIndices.size());*/
}

void UIManager::cleanUpBuffers(VulkanCoreInfo* vulkanCoreInfo)
{
    vkDestroyBuffer(vulkanCoreInfo->device, vertexBuffer, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, vertexBufferMemory, nullptr);

    vkDestroyBuffer(vulkanCoreInfo->device, indexBuffer, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, indexBufferMemory, nullptr);
}