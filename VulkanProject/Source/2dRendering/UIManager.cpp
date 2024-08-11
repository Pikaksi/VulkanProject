#include "UIManager.hpp"

#include <chrono>
#include <vector>

#include "PlayerInputHandler.hpp"

void UIManager::updateScreen(VkExtent2D extent, VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, VertexBufferManager& vertexBufferManager)
{
	if (hasAllocatedGPUBlockMemory) {
		vertexBufferManager.freeUIVerticesMemory(uiMemoryPointer);
		hasAllocatedGPUBlockMemory = false;
	}

	uiMemoryPointer = vertexBufferManager.addVerticesToUI(vulkanCoreInfo, commandPool, vertices);
	hasAllocatedGPUBlockMemory = true;
}

VkExtent2D UIManager::getExtent()
{
	return extent;
}

void UIManager::changeExtent(VkExtent2D newExtent)
{
	extent = newExtent;
}