#include "UIQuad.hpp"

#include <iostream>

void UIQuad::addMeshData(int screenWidth, int screenHeight, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) const
{
    float xScale = 1.0f;
    float yScale = 1.0f;
    if (screenHeight >= screenWidth) {
        yScale = (float)screenWidth / (float)screenHeight;
    }
    else {
        xScale = (float)screenHeight / (float)screenWidth;
    }
    uint32_t vertexCount = vertices.size();

    indices.push_back(vertexCount);
    indices.push_back(vertexCount + 1);
    indices.push_back(vertexCount + 2);
    indices.push_back(vertexCount + 2);
    indices.push_back(vertexCount + 3);
    indices.push_back(vertexCount);

    /*std::cout
        << "width = " << screenWidth << " height = " << screenHeight << "\n"
        << "xScale = " << xScale << "\n"
        << "x = " << x << " y = " << y << "\n"
        << "x = " << x << " y = " << y + (height * yScale) << "\n"
        << "x = " << x + (width * xScale) << " y = " << y + (height * yScale) << "\n"
        << "x = " << x + (width * xScale) << " y = " << y << "\n";*/
    vertices.push_back(Vertex{ {x,                    y,                     0}, {1.0f, 1.0f, 1.0f}, {texX, texY}, 0.0f });
    vertices.push_back(Vertex{ {x,                    y + (height * yScale), 0}, {1.0f, 1.0f, 1.0f}, {texX, texY + texHeight}, 0.0f });
    vertices.push_back(Vertex{ {x + (width * xScale), y + (height * yScale), 0}, {1.0f, 1.0f, 1.0f}, {texX + texWidth, texY + texHeight}, 0.0f });
    vertices.push_back(Vertex{ {x + (width * xScale), y,                     0}, {1.0f, 1.0f, 1.0f}, {texX + texWidth, texY}, 0.0f });
}
