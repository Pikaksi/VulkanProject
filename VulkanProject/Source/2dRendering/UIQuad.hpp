#pragma once

#include <vector>

#include "Rendering/Vertex.hpp"

enum class UICenteringMode
{
	BottomLeft = 0,
	Bottom = 1,
	BottomRight = 2,
	Left = 3,
	Center = 4,
	Right = 5,
	TopLeft = 6,
	Top = 7,
	TopRight = 8,
};

struct UIQuad
{
public:
	glm::vec2 location;
	glm::vec2 size;
	glm::vec2 texDownLeft;
	glm::vec2 texUpRight;
	float texLayer;
	glm::vec4 color;
	UICenteringMode centeringMode;

	UIQuad() {};
	UIQuad(glm::vec2 location, glm::vec2 size, glm::vec2 texDownLeft, glm::vec2 texUpRight, float texLayer, glm::vec4 color, UICenteringMode centeringMode) :
		location(location), size(size), texDownLeft(texDownLeft), texUpRight(texUpRight), texLayer(texLayer), color(color), centeringMode(centeringMode) {}


	void addMeshData(VkExtent2D extent, std::vector<Vertex2D>& vertices) const;
};
