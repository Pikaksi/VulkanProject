#pragma once

#include <vector>

#include "UIObject.hpp"
#include "Rendering/Vertex.hpp"
#include "UITexLayer.hpp"
#include "UICenteringMode.hpp"

struct UIQuad : public UIObject
{
public:
	glm::vec2 location;
	glm::vec2 size;
	glm::vec2 texDownLeft;
	glm::vec2 texUpRight;
	uint32_t texLayer;
	glm::vec4 color;
	UICenteringMode centeringMode;
	bool automaticResize;

	UIQuad() {};
	UIQuad(glm::vec2 location, glm::vec2 size, glm::vec2 texDownLeft, glm::vec2 texUpRight, UITexLayer texLayer, glm::vec4 color, UICenteringMode centeringMode, bool automaticResize) :
		location(location), size(size), texDownLeft(texDownLeft), texUpRight(texUpRight), texLayer((uint32_t)texLayer), color(color), centeringMode(centeringMode), automaticResize(automaticResize) {}

	void locationAdd(glm::vec2 vector) { location += vector; }
	void locationMul(glm::vec2 vector) { location *= vector; }
	void sizeAdd(glm::vec2 vector) { size += vector; }
	void sizeMul(glm::vec2 vector) { size *= vector; }

	void addMeshData(VkExtent2D extent, std::vector<Vertex2D>& vertices);
};
