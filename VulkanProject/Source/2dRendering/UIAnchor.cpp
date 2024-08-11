#include "UIAnchor.hpp"

#include <iostream>

#include "UIHelperFunctions.hpp"

UIAnchor::UIAnchor(glm::vec2 locationPreScaling, glm::vec2 sizePreScaling, UICenteringMode centeringMode, VkExtent2D extent)
{
    glm::vec2 scalar = getScalarFromExtent(extent);
    size = sizePreScaling * scalar;

    location = getCenteredLocation(locationPreScaling, size, centeringMode);
}

glm::vec2 UIAnchor::getAnchorPointLocation(UICenteringMode uiCenteringMode)
{
    return getCenteredLocation(location, size, uiCenteringMode);
}

void UIAnchor::scaleToAnchor(glm::vec2& locationToScale, glm::vec2& sizeToScale)
{
    scaleBoxToWindow(location, size, locationToScale, sizeToScale);
}