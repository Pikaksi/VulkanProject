#include "UIManager.hpp"

#include <iostream>

void UIManager::refreshUI(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, int screenWidth, int screenHeight)
{
    if (!uIRefreshNeeded) {
        return;
    }
    uIRefreshNeeded = false;

    updatedUIIndex = (updatedUIIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    cleanUpSingleFrame(vulkanCoreInfo, updatedUIIndex);

    std::vector<Vertex> newVertices;
    std::vector<uint32_t> newIndices;

    for (int i = 0; i < currentUIElements.size(); i++) {
        currentUIElements[i].addMeshData(screenWidth, screenHeight, newVertices, newIndices);
    }
    for (int i = 0; i < currentTextElements.size(); i++) {
        currentTextElements[i].addMeshData(screenWidth, screenHeight, newVertices, newIndices);
    }

    bool hasVertices = newVertices.size() > 0;
    hasElementsToRender[updatedUIIndex] = hasVertices;
    std::cout << "refreshed with vertex count = " << (newVertices.size() > 0) << "\n";
    if (!hasVertices) {
        return;
    }

    createVertexBuffer(vulkanCoreInfo, commandPool, vertexBuffer[updatedUIIndex], vertexBufferMemory[updatedUIIndex], newVertices);
    createIndexBuffer(vulkanCoreInfo, commandPool, indexBuffer[updatedUIIndex], indexBufferMemory[updatedUIIndex], newIndices);
    indexCount[updatedUIIndex] = newIndices.size();

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

UIText* UIManager::createUIText(float x, float y, float letterHeight, std::string text)
{
    UIText uIText(x, y, letterHeight, text);
    currentTextElements.push_back(uIText);
    return currentTextElements.data() + currentTextElements.size() - 1;
}

void UIManager::deleteUIText(UIText* uIText)
{
    for (int i = 0; i < currentTextElements.size(); i++) {
        if (uIText == currentTextElements.data() + i) {
            std::cout << "deleted at " << i << "\n";
            currentTextElements.erase(currentTextElements.begin() + i);
            uIRefreshNeeded = true;
        }
    }
}

void UIManager::cleanUpSingleFrame(VulkanCoreInfo* vulkanCoreInfo, int index)
{
    vkDestroyBuffer(vulkanCoreInfo->device, vertexBuffer[index], nullptr);
    vkFreeMemory(vulkanCoreInfo->device, vertexBufferMemory[index], nullptr);

    vkDestroyBuffer(vulkanCoreInfo->device, indexBuffer[index], nullptr);
    vkFreeMemory(vulkanCoreInfo->device, indexBufferMemory[index], nullptr);
}

void UIManager::cleanUpAll(VulkanCoreInfo* vulkanCoreInfo)
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(vulkanCoreInfo->device, vertexBuffer[i], nullptr);
        vkFreeMemory(vulkanCoreInfo->device, vertexBufferMemory[i], nullptr);

        vkDestroyBuffer(vulkanCoreInfo->device, indexBuffer[i], nullptr);
        vkFreeMemory(vulkanCoreInfo->device, indexBufferMemory[i], nullptr);
    }
}