#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Rendering/Vertex.hpp"

class UIObject
{
public:
	virtual void addMeshData(VkExtent2D extent, std::vector<Vertex2D>& vertices) = 0;
	virtual ~UIObject() {}
};