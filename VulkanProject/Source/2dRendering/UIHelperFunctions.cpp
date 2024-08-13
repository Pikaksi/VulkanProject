#include "UIHelperFunctions.hpp"

#include <iostream>

glm::vec2 getScalarFromExtent(VkExtent2D extent)
{
    glm::vec2 sizeScalar = { 1.0f, 1.0f };
    if (extent.height >= extent.width) {
        sizeScalar.y = (float)extent.width / (float)extent.height;
    }
    else {
        sizeScalar.x = (float)extent.height / (float)extent.width;
    }
    return sizeScalar;
}

void centerLocation(glm::vec2& location, const glm::vec2 size, const UICenteringMode centeringMode)
{
    if (centeringMode == UICenteringMode::top || centeringMode == UICenteringMode::center || centeringMode == UICenteringMode::bottom) {
        location.x -= size.x / 2;
    }
    else if (centeringMode == UICenteringMode::bottomRight || centeringMode == UICenteringMode::right || centeringMode == UICenteringMode::topRight) {
        location.x -= size.x;
    }

    if (centeringMode == UICenteringMode::left || centeringMode == UICenteringMode::center || centeringMode == UICenteringMode::right) {
        location.y -= size.y / 2;
    }
    else if (centeringMode == UICenteringMode::bottomLeft || centeringMode == UICenteringMode::bottom || centeringMode == UICenteringMode::bottomRight) {
        location.y -= size.y;
    }
}

glm::vec2 getCenteredLocation(const glm::vec2 location, const glm::vec2 size, const UICenteringMode centeringMode)
{
    glm::vec2 centeredLocation = location;

    if (centeringMode == UICenteringMode::top || centeringMode == UICenteringMode::center || centeringMode == UICenteringMode::bottom) {
        centeredLocation.x -= size.x / 2;
    }
    else if (centeringMode == UICenteringMode::bottomRight || centeringMode == UICenteringMode::right || centeringMode == UICenteringMode::topRight) {
        centeredLocation.x -= size.x;
    }

    if (centeringMode == UICenteringMode::left || centeringMode == UICenteringMode::center || centeringMode == UICenteringMode::right) {
        centeredLocation.y -= size.y / 2;
    }
    else if (centeringMode == UICenteringMode::bottomLeft || centeringMode == UICenteringMode::bottom || centeringMode == UICenteringMode::bottomRight) {
        centeredLocation.y -= size.y;
    }
    return centeredLocation;
}

void scaleBoxToWindow(glm::vec2 windowLocation, glm::vec2 windowSize, glm::vec2& location, glm::vec2& size)
{
    size *= windowSize * 0.5f;
    
    location *= windowSize * 0.5f;
    location += windowLocation + windowSize * 0.5f;
}

void scaleBoxToWindow(const glm::vec2 windowLocation, const glm::vec2 windowSize, InventorySlotLocation& slotLocation)
{
    slotLocation.size *= windowSize * 0.5f;
    
    slotLocation.location *= windowSize * 0.5f;
    slotLocation.location += windowLocation + windowSize * 0.5f;
}

bool isLocationInBox(const glm::vec2 point, const glm::vec2 location, const glm::vec2 size)
{
    return point.x >= location.x && point.x <= location.x + size.x && point.y >= location.y && point.y <= location.y + size.y;
}
