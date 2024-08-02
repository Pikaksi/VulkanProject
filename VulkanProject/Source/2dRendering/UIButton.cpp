#include "UIButton.hpp"

#include "UIHelperFunctions.hpp"

bool UIButton::updateStatus(VkExtent2D extent, glm::vec2 mouseLocation)
{
    glm::vec2 realSize = size;

    if (automaticResize) {
        glm::vec2 scalar = getScalarFromExtent(extent);
        realSize = size * scalar;
    }

    glm::vec2 centeredLocation = getCenteredLocation(location, realSize, centeringMode);

    if (centeredLocation.x < mouseLocation.x && centeredLocation.y < mouseLocation.y &&
        centeredLocation.x + realSize.x > mouseLocation.x && centeredLocation.y + realSize.y > mouseLocation.y)
    {
        // The number here does not matter.
        // It is replaced by the number given during the construction of the function.
        callbackFunction(0);
        return true;
    }
    return false;
}