#include "UIQuad.hpp"

#include <iostream>

void UIQuad::addMeshData(VkExtent2D extent, std::vector<Vertex2D>& vertices) const
{
    float xScale = 1.0f;
    float yScale = 1.0f;
    if (extent.height >= extent.width) {
        yScale = (float)extent.width / (float)extent.height;
    }
    else {
        xScale = (float)extent.height / (float)extent.width;
    }

    glm::vec2 realSize = { size.x * xScale, size.y * yScale };
    glm::vec2 locationCentered = location;

    if (centeringMode == UICenteringMode::Top || centeringMode == UICenteringMode::Center || centeringMode == UICenteringMode::Bottom) {
        locationCentered.x -= realSize.x / 2;
    }
    else if (centeringMode == UICenteringMode::BottomRight || centeringMode == UICenteringMode::Right || centeringMode == UICenteringMode::TopRight) {
        locationCentered.x -= realSize.x;
    }

    if (centeringMode == UICenteringMode::Left || centeringMode == UICenteringMode::Center || centeringMode == UICenteringMode::Right) {
        locationCentered.y -= realSize.y / 2;
    }
    else if (centeringMode == UICenteringMode::TopLeft || centeringMode == UICenteringMode::Top || centeringMode == UICenteringMode::TopRight) {
        //locationCentered.y -= realSize.y;
    }
    vertices.push_back(Vertex2D{ {locationCentered.x,              locationCentered.y,            }, color, texDownLeft,                   0.0f });
    vertices.push_back(Vertex2D{ {locationCentered.x,              locationCentered.y + realSize.y}, color, { texDownLeft.x, texUpRight.y }, 0.0f });
    vertices.push_back(Vertex2D{ {locationCentered.x + realSize.x, locationCentered.y + realSize.y}, color, texUpRight,                    0.0f });
    vertices.push_back(Vertex2D{ {locationCentered.x + realSize.x, locationCentered.y,            }, color, {texUpRight.x, texDownLeft.y}, 0.0f });
}
