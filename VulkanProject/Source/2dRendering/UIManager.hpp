#pragma once

#include <vector>

#include "Rendering/VertexBufferManager.hpp"

// All components derive from UIObject.
// To create a UIObject, create it with the given functions.
// If you edit the UIObject call updateUIObject. (Even if you change it right after creation.)
// Destroy the object with destroyUIObject.
class UIManager
{
public:
	UIManager() {};

	glm::vec2 getScalar();
	VkExtent2D getExtent();
	std::vector<Vertex2D>& getVertexVector();

	void updateScreen(VkExtent2D extent, VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, VertexBufferManager& vertexBufferManager);
	void changeExtent(VkExtent2D newExtent);

private:
	glm::vec2 scalar;
	bool hasAllocatedGPUBlockMemory = false;
	uint32_t uiMemoryPointer = 0;
	VkExtent2D extent;
	std::vector<Vertex2D> vertices;
};