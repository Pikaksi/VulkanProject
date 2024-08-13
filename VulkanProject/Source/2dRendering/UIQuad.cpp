#include "UIQuad.hpp"

void createUIQuad(
    UIManager& uiManager,
    glm::vec2 location,
    glm::vec2 size,
    glm::vec2 texBottomLeft,
    glm::vec2 texTopRight,
    UITexLayer texLayer,
    glm::vec4 color)
{
    std::vector<Vertex2D>& vertices = uiManager.getVertexVector();
    vertices.push_back(Vertex2D{ {location.x,          location.y,        }, color, texBottomLeft,                         (uint32_t)texLayer });
    vertices.push_back(Vertex2D{ {location.x,          location.y + size.y}, color, { texBottomLeft.x, texTopRight.y }, (uint32_t)texLayer });
    vertices.push_back(Vertex2D{ {location.x + size.x, location.y + size.y}, color, texTopRight,                     (uint32_t)texLayer });
    vertices.push_back(Vertex2D{ {location.x + size.x, location.y,        }, color, { texTopRight.x, texBottomLeft.y }, (uint32_t)texLayer });
}
