#pragma once

#include <vector>
#include <map>
#include <set>

#include "UIQuad.hpp"
#include "UIText.hpp"
#include "Rendering/VertexBufferManager.hpp"

// Create a UI compotent with createUIObject. 
// Make changes to set object.
// Call updateUIObject. (Even if you change it right after creation.)
// Destroy the object with destroyUIObject.
class UIManager
{
public:
	bool screenSizeChanged = false;

	UIManager() {};

	void updateScreen(VkExtent2D extent, VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VertexBufferManager& vertexBufferManager);

	UIQuad* createUIQuad();
	UIQuad* createUIQuad(glm::vec2 location, glm::vec2 size, glm::vec2 texDownLeft, glm::vec2 texUpRight, float texLayer, glm::vec4 color, UICenteringMode uiCenteringMode);
	void updateUIQuad(UIQuad* uiQuad);
	void deleteUIQuad(UIQuad* uiQuad);

	UIText* createUIText();
	UIText* createUIText(glm::vec2 location, float letterHeight, std::string text, UICenteringMode letterCenteringMode);
	void updateUIText(UIText* uiText);
	void deleteUIText(UIText* uiText);

	void cleanup();

private:
	void removeUIQuad(UIQuad* uiQuad);
	void removeUIText(UIText* uiText);

	std::set<UIQuad*> quadsAllocated;
	std::vector<UIQuad*> quadsToRender;
	std::map<UIQuad*, uint32_t> quadsRendered;
	std::vector<uint32_t> quadsToDerender;

	std::set<UIText*> textAllocated;
	std::vector<UIText*> textToRender;
	std::map<UIText*, uint32_t> textRendered;
	std::vector<uint32_t> textToDerender;
};