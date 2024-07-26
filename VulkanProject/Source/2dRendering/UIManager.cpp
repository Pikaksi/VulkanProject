#include "UIManager.hpp"

#include <vector>


void UIManager::updateScreen(VkExtent2D extent, VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, VertexBufferManager& vertexBufferManager)
{
	for (int i = 0; i < quadsToDerender.size(); i++) {
		vertexBufferManager.freeUIVerticesMemory(quadsToDerender[i]);
	}
	quadsToDerender.clear();
	for (int i = 0; i < textToDerender.size(); i++) {
		vertexBufferManager.freeUIVerticesMemory(textToDerender[i]);
	}
	textToDerender.clear();


	for (int i = 0; i < quadsToRender.size(); i++) {
		UIQuad* uiQuad = quadsToRender[i];

		std::vector<Vertex2D> vertices;
		uiQuad->addMeshData(extent, vertices);
		uint32_t memoryLocation = vertexBufferManager.addVerticesToUI(vulkanCoreInfo, commandPool, vertices);
		quadsRendered.insert(std::make_pair(uiQuad, memoryLocation));
	}
	quadsToRender.clear();

	for (int i = 0; i < textToRender.size(); i++) {
		UIText* uiText = textToRender[i];

		std::vector<Vertex2D> vertices;
		uiText->addMeshData(extent, vertices);
		uint32_t memoryLocation = vertexBufferManager.addVerticesToUI(vulkanCoreInfo, commandPool, vertices);
		textRendered.insert(std::make_pair(uiText, memoryLocation));
	}
	textToRender.clear();
}

UIQuad* UIManager::createUIQuad()
{
	UIQuad* uiQuad = new UIQuad();
	quadsAllocated.insert(uiQuad);
	return uiQuad;
}

UIQuad* UIManager::createUIQuad(glm::vec2 location, glm::vec2 size, glm::vec2 texDownLeft, glm::vec2 texUpRight, uint32_t texLayer, glm::vec4 color, UICenteringMode uiCenteringMode)
{
	UIQuad* uiQuad = new UIQuad(location, size, texDownLeft, texUpRight, texLayer, color, uiCenteringMode);
	quadsAllocated.insert(uiQuad);
	return uiQuad;
}

void UIManager::updateUIQuad(UIQuad* uiQuad)
{
	removeUIQuad(uiQuad);
	quadsToRender.push_back(uiQuad);
}

void UIManager::deleteUIQuad(UIQuad* uiQuad)
{
	removeUIQuad(uiQuad);
	delete uiQuad;
}

void UIManager::removeUIQuad(UIQuad* uiQuad)
{
	bool quadIsRendered = quadsRendered.contains(uiQuad);
	bool quadIsAllocated = quadsAllocated.contains(uiQuad);

#ifndef NDEBUG
	if (!quadIsRendered && !quadIsAllocated) {
		throw std::runtime_error("Tried to update UIQuad but it does not exist");
	}
#endif

	if (quadIsRendered) {
		quadsToDerender.push_back(quadsRendered.at(uiQuad));
		quadsRendered.erase(uiQuad);
	}
	else if (quadIsAllocated) {
		quadsAllocated.erase(uiQuad);
	}
}

UIText* UIManager::createUIText()
{
	UIText* uiText = new UIText();
	textAllocated.insert(uiText);
	return uiText;
}

UIText* UIManager::createUIText(glm::vec2 location, float letterHeight, std::string text, UICenteringMode letterCenteringMode)
{
	UIText* uiText = new UIText(location, letterHeight, text, letterCenteringMode);
	textAllocated.insert(uiText);
	return uiText;
}

void UIManager::updateUIText(UIText* uiText)
{
	removeUIText(uiText);
	textToRender.push_back(uiText);
}

void UIManager::deleteUIText(UIText* uiText)
{
	removeUIText(uiText);
	delete uiText;
}

void UIManager::removeUIText(UIText* uiText)
{
	bool textIsRendered = textRendered.contains(uiText);
	bool textIsAllocated = textAllocated.contains(uiText);

#ifndef NDEBUG
	if (!textIsRendered && !textIsAllocated) {
		throw std::runtime_error("Tried to update UIText but it does not exist");
	}
#endif

	if (textIsRendered) {
		textToDerender.push_back(textRendered.at(uiText));
		textRendered.erase(uiText);
	}
	else if (textIsAllocated) {
		textAllocated.erase(uiText);
	}
}

void UIManager::cleanup()
{
	for (auto uiObjectPointer : quadsAllocated) {
		delete uiObjectPointer;
	}
	for (auto uiObjectPointer : quadsToRender) {
		delete uiObjectPointer;
	}
	for (auto uiObjectPointer : quadsRendered) {
		delete uiObjectPointer.first;
	}
	for (auto uiObjectPointer : textAllocated) {
		delete uiObjectPointer;
	}
	for (auto uiObjectPointer : textToRender) {
		delete uiObjectPointer;
	}
	for (auto uiObjectPointer : textRendered) {
		delete uiObjectPointer.first;
	}
}