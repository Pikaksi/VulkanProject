#pragma once

#include "vulkan/vulkan.h"

#include "VulkanRendering/Buffers.hpp"

class GPUMemoryBlock
{
	VkBuffer buffer;
	VkDeviceMemory memory;
};