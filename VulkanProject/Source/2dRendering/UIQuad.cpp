#include "UIQuad.hpp"

#include <iostream>

#include "UIHelperFunctions.hpp"

void UIQuad::addMeshData(VkExtent2D extent, std::vector<Vertex2D>& vertices)
{
    glm::vec2 realSize = size;

    if (automaticResize) {
        glm::vec2 scalar = getScalarFromExtent(extent);
        realSize = size * scalar;
    }

    glm::vec2 centeredLocation = getCenteredLocation(location, realSize, centeringMode);

    vertices.push_back(Vertex2D{ {centeredLocation.x,              centeredLocation.y,            }, color, texDownLeft,                     texLayer });
    vertices.push_back(Vertex2D{ {centeredLocation.x,              centeredLocation.y + realSize.y}, color, { texDownLeft.x, texUpRight.y }, texLayer });
    vertices.push_back(Vertex2D{ {centeredLocation.x + realSize.x, centeredLocation.y + realSize.y}, color, texUpRight,                      texLayer });
    vertices.push_back(Vertex2D{ {centeredLocation.x + realSize.x, centeredLocation.y,            }, color, { texUpRight.x, texDownLeft.y }, texLayer });
}
