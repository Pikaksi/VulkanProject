#pragma once

#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>

#include "UICenteringMode.hpp"
#include "Inventory/InventoryLayouts.hpp"

void centerLocation(glm::vec2& location, const glm::vec2 size, const UICenteringMode centeringMode);
glm::vec2 getCenteredLocation(const glm::vec2 location, const glm::vec2 size, const UICenteringMode centeringMode);

void scaleBoxToWindow(const glm::vec2 windowLocation, const glm::vec2 windowSize, glm::vec2& location, glm::vec2& size);
void scaleBoxToWindow(const glm::vec2 windowLocation, const glm::vec2 windowSize, InventorySlotLocation& slotLocation);
bool isLocationInBox(const glm::vec2 point, const glm::vec2 location, const glm::vec2 size);

glm::vec2 getScalarFromExtent(VkExtent2D extent);