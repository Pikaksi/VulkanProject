#pragma once

#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>
#include "UICenteringMode.hpp"

glm::vec2 getScalarFromExtent(VkExtent2D extent);
glm::vec2 getCenteredLocation(glm::vec2 location, glm::vec2 size, UICenteringMode centeringMode);
void scaleBoxToWindow(glm::vec2 windowLocation, glm::vec2 windowSize, glm::vec2& location, glm::vec2& size);