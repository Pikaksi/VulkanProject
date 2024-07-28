#include "UIManager.hpp"

#include <vector>


void UIManager::updateScreen(VkExtent2D extent, VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, VertexBufferManager& vertexBufferManager)
{
	for (int i = 0; i < uiObjectsToDerender.size(); i++) {
		vertexBufferManager.freeUIVerticesMemory(uiObjectsToDerender[i]);
	}
	uiObjectsToDerender.clear();

	for (int i = 0; i < uiObjectsToRender.size(); i++) {
		UIObject* uiObject = uiObjectsToRender[i];

		std::vector<Vertex2D> vertices;
		uiObject->addMeshData(extent, vertices);
		uint32_t memoryLocation = vertexBufferManager.addVerticesToUI(vulkanCoreInfo, commandPool, vertices);
		uiObjectsRendered.insert(std::make_pair(uiObject, memoryLocation));
	}
	uiObjectsToRender.clear();
}

void UIManager::rerenderIfExtentChanged(VkExtent2D extent, VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, VertexBufferManager& vertexBufferManager)
{
	if (!extentChanged) {
		return;
	}

	for (std::pair<UIObject*, uint32_t> pair : uiObjectsRendered) {
		uiObjectsToRender.push_back(pair.first);
		uiObjectsToDerender.push_back(pair.second);
	}
	uiObjectsRendered.clear();

	updateScreen(extent, vulkanCoreInfo, commandPool, vertexBufferManager);
}

void UIManager::updateUIObject(UIObject* uiObject)
{
	removeUIObject(uiObject);
	uiObjectsToRender.push_back(uiObject);
}

void UIManager::deleteUIObject(UIObject* uiObject)
{
	removeUIObject(uiObject);
	delete uiObject;
}

void UIManager::removeUIObject(UIObject* uiObject)
{
	bool uiObjectIsRendered = uiObjectsRendered.contains(uiObject);
	bool uiObjectIsAllocated = uiObjectsAllocated.contains(uiObject);

	if (!uiObjectIsRendered && !uiObjectIsAllocated) {
		std::cout << "Updated a UIObject twice in a frame or bug\n";
		return;
	}

	if (uiObjectIsRendered) {
		uiObjectsToDerender.push_back(uiObjectsRendered.at(uiObject));
		uiObjectsRendered.erase(uiObject);
	}
	else if (uiObjectIsAllocated) {
		uiObjectsAllocated.erase(uiObject);
	}
}

UIQuad* UIManager::createUIQuad()
{
	UIQuad* uiQuad = new UIQuad();
	uiObjectsAllocated.insert(uiQuad);
	return uiQuad;
}

UIQuad* UIManager::createUIQuad(glm::vec2 location, glm::vec2 size, glm::vec2 texDownLeft, glm::vec2 texUpRight, UITexLayer texLayer, glm::vec4 color, UICenteringMode uiCenteringMode, bool automaticResize)
{
	UIQuad* uiQuad = new UIQuad(location, size, texDownLeft, texUpRight, texLayer, color, uiCenteringMode, automaticResize);
	uiObjectsAllocated.insert(uiQuad);
	return uiQuad;
}


UIText* UIManager::createUIText()
{
	UIText* uiText = new UIText();
	uiObjectsAllocated.insert(uiText);
	return uiText;
}

UIText* UIManager::createUIText(glm::vec2 location, float letterHeight, std::string text, UICenteringMode letterCenteringMode)
{
	UIText* uiText = new UIText(location, letterHeight, text, letterCenteringMode);
	uiObjectsAllocated.insert(uiText);
	return uiText;
}

DefaultWindow* UIManager::createDefaultWindow()
{
	DefaultWindow* defaultWindow = new DefaultWindow();
	uiObjectsAllocated.insert(defaultWindow);
	return defaultWindow;
}

DefaultWindow* UIManager::createDefaultWindow(glm::vec2 location, glm::vec2 size, UICenteringMode uiCenteringMode, float topBarHeight, std::string title, glm::vec4 topBarColor, glm::vec4 bodyColor)
{
	DefaultWindow* defaultWindow = new DefaultWindow(location, size, uiCenteringMode, topBarHeight, title, topBarColor, bodyColor);
	uiObjectsAllocated.insert(defaultWindow);
	return defaultWindow;
}

void UIManager::cleanup()
{
	for (auto uiObjectPointer : uiObjectsAllocated) {
		delete uiObjectPointer;
	}
	for (auto uiObjectPointer : uiObjectsToRender) {
		delete uiObjectPointer;
	}
	for (auto uiObjectPointer : uiObjectsRendered) {
		delete uiObjectPointer.first;
	}
}