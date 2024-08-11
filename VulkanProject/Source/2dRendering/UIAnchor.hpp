#pragma once

#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>

#include "UICenteringMode.hpp"

// Used for seeing where UIObjects end up after automatic scaling.
class UIAnchor
{
public:
    glm::vec2 location;
    glm::vec2 size;

    UIAnchor(glm::vec2 locationPreScaling, glm::vec2 sizePreScaling, UICenteringMode centeringMode, VkExtent2D extent);
    
    glm::vec2 getAnchorPointLocation(UICenteringMode uiCenteringMode);
    void scaleToAnchor(glm::vec2& location, glm::vec2& size);
};