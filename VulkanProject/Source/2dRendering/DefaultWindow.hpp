#pragma once

#include <string>

#include "UIText.hpp"

class DefaultWindow : public UIObject
{
public:
	void addMeshData(VkExtent2D extent, std::vector<Vertex2D>& vertices);
	void addMeshDataScaledCentered(VkExtent2D extent, std::vector<Vertex2D>& vertices, glm::vec2 location, glm::vec2 size, float topBarHeight);

	DefaultWindow() {}
	DefaultWindow(glm::vec2 location, glm::vec2 size, UICenteringMode uiCenteringMode, float topBarHeight, std::string title, glm::vec4 topBarColor, glm::vec4 bodyColor)
		: location(location), size(size), centeringMode(uiCenteringMode), topBarHeight(topBarHeight), topBarColor(topBarColor), bodyColor(bodyColor)
	{
		this->title = title;
	}

private:
	glm::vec2 location;
	glm::vec2 size;
	UICenteringMode centeringMode;
	float topBarHeight;
	std::string title;
	glm::vec4 topBarColor;
	glm::vec4 bodyColor;
};