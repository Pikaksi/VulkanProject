#pragma once

#include <vector>
#include <map>
#include <set>
#include <functional>

#include "UIQuad.hpp"
#include "UIText.hpp"
#include "DefaultWindow.hpp"
#include "UIButton.hpp"
#include "Rendering/VertexBufferManager.hpp"

// All components derive from UIObject.
// To create a UIObject, create it with the given functions.
// If you edit the UIObject call updateUIObject. (Even if you change it right after creation.)
// Destroy the object with destroyUIObject.
class UIManager
{
public:
	void updateUIObject(UIObject* uiObject);
	void destroyUIObject(UIObject* uiObject);

	UIQuad* createUIQuad();
	UIQuad* createUIQuad(glm::vec2 location, glm::vec2 size, glm::vec2 texDownLeft, glm::vec2 texUpRight, UITexLayer texLayer, glm::vec4 color, UICenteringMode uiCenteringMode, bool automaticResize);

	UIText* createUIText();
	UIText* createUIText(glm::vec2 location, float letterHeight, std::string text, UICenteringMode letterCenteringMode);

	DefaultWindow* createDefaultWindow();
	DefaultWindow* createDefaultWindow(glm::vec2 location, glm::vec2 size, UICenteringMode uiCenteringMode, float topBarHeight, std::string title, glm::vec4 topBarColor, glm::vec4 bodyColor);

	UIButton* createUIButton();
	UIButton* createUIButton(glm::vec2 location, glm::vec2 size, UICenteringMode centeringMode, bool automaticResize, std::function<void(int)> callbackFunction, int callbackNumber);
	void destroyUIButton(UIButton* uiButton);

	UIManager() {};
	void updateScreen(VkExtent2D extent, VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, VertexBufferManager& vertexBufferManager);
	void updateButtons(VkExtent2D extent);
	void cleanup();

	VkExtent2D getExtent();
	void changeExtent(VkExtent2D newExtent);
	void rerenderIfExtentChanged(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, VertexBufferManager& vertexBufferManager);

private:
	void removeUIObject(UIObject* uiQuad);

	bool extentChanged = false;
	VkExtent2D extent;

	std::set<UIObject*> uiObjectsAllocated;
	std::vector<UIObject*> uiObjectsToRender;
	std::map<UIObject*, uint32_t> uiObjectsRendered;
	std::vector<uint32_t> uiObjectsToDerender;

	std::set<UIButton*> uiButtons;
};