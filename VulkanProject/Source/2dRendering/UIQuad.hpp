#pragma once

#include <vector>

#include "3dRendering/Vertex.hpp"

class UIQuad
{
public:
	float x;
	float y;
	float width;
	float height;

	UIQuad(float x, float y, float width, float height)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

	void addMeshData(int screenWidth, int screenHeight, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
};
