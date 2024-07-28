#include "UIHelperFunctions.hpp"

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

glm::vec2 getCenteredLocation(glm::vec2 location, glm::vec2 size, UICenteringMode centeringMode)
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