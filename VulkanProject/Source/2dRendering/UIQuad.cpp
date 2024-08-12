#include "UIQuad.hpp"

void createUIQuad(
    UIManager& uiManager,
    glm::vec2 location,
    glm::vec2 size,
    glm::vec2 texTopLeft,
    glm::vec2 texBottomRight,
    UITexLayer texLayer,
    glm::vec4 color)
{
    std::vector<Vertex2D>& vertices = uiManager.getVertexVector();
    vertices.push_back(Vertex2D{ {location.x,          location.y,        }, color, texTopLeft,                         (uint32_t)texLayer });
    vertices.push_back(Vertex2D{ {location.x,          location.y + size.y}, color, { texBottomRight.x, texTopLeft.y }, (uint32_t)texLayer });
    vertices.push_back(Vertex2D{ {location.x + size.x, location.y + size.y}, color, texBottomRight,                     (uint32_t)texLayer });
    vertices.push_back(Vertex2D{ {location.x + size.x, location.y,        }, color, { texTopLeft.x, texBottomRight.y }, (uint32_t)texLayer });
}
