#pragma once

#include <vector>

#include "Vertex.hpp"

class VertexBufferManager
{

public:
    // all the three vectors should be kept parallel at all times

    std::vector<uint32_t> vertexCounts;
    std::vector<VkBuffer> vertexBuffers;
    std::vector<VkDeviceMemory> vertexBufferMemories;

    std::vector<uint32_t> indexCounts;
    std::vector<VkBuffer> indexBuffers;
    std::vector<VkDeviceMemory> indexBufferMemories;


    VertexBufferManager()
    {
    }

    void addChunkVertices(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    void createVertexBuffer(VkBuffer& buffer, VkDeviceMemory& memory, std::vector<Vertex>& vertices);
    void createIndexBuffer(VkBuffer& buffer, VkDeviceMemory& memory, std::vector<uint32_t>& indices);

    ~VertexBufferManager();

};

#include "Application.hpp"