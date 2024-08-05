#pragma once

#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>

#include <functional>

#include "UICenteringMode.hpp"

class UIButton
{
public:
	glm::vec2 location;
	glm::vec2 size;
	UICenteringMode centeringMode;
	bool automaticResize;
	std::function<void(int)> callbackFunction;
	int callbackNumber;

	UIButton() {}
	UIButton(glm::vec2 location, glm::vec2 size, UICenteringMode centeringMode, bool automaticResize, std::function<void(int)> callbackFunction, int callbackNumber)
		: location(location), size(size), centeringMode(centeringMode), automaticResize(automaticResize), callbackFunction(callbackFunction), callbackNumber(callbackNumber) {}

	bool updateStatus(VkExtent2D extent, glm::vec2 mouseLocation);
};