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
	float texX; 
	float texY; 
	float texWidth; 
	float texHeight;

	UIQuad(float x, float y, float width, float height, float texX, float texY, float texWidth, float texHeight)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->texX = texX;
		this->texY = texY;
		this->texWidth = texWidth;
		this->texHeight = texHeight;
	}

	void addMeshData(int screenWidth, int screenHeight, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) const;
};
