#pragma once

#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>
#include "UICenteringMode.hpp"

glm::vec2 getScalarFromExtent(VkExtent2D extent);
glm::vec2 getCenteredLocation(glm::vec2 location, glm::vec2 size, UICenteringMode centeringMode);