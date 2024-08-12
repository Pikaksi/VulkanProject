#include "UIManager.hpp"

#include <chrono>
#include <vector>

#include "UIHelperFunctions.hpp"
#include "PlayerInputHandler.hpp"

void UIManager::updateScreen(VkExtent2D extent, VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, VertexBufferManager& vertexBufferManager)
{
	if (hasAllocatedGPUBlockMemory) {
		vertexBufferManager.freeUIVerticesMemory(uiMemoryPointer);
		hasAllocatedGPUBlockMemory = false;
	}

	if (!hasAllocatedGPUBlockMemory && vertices.size() != 0) {
		uiMemoryPointer = vertexBufferManager.addVerticesToUI(vulkanCoreInfo, commandPool, vertices);
		vertices.clear();
		hasAllocatedGPUBlockMemory = true;
	}
}

std::vector<Vertex2D>& UIManager::getVertexVector()
{
	return vertices;
}

glm::vec2 UIManager::getScalar()
{
	return scalar;
}

VkExtent2D UIManager::getExtent()
{
	return extent;
}

void UIManager::changeExtent(VkExtent2D newExtent)
{
	extent = newExtent;
	scalar = getScalarFromExtent(newExtent);
}