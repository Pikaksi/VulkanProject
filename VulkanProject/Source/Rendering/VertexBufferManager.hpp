#pragma once

#include <vector>

#include "Vertex.hpp"
#include "VulkanRendering/VulkanTypes.hpp"
#include "VulkanRendering/Buffers.hpp"

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

    std::vector<Vertex> testVertices;
    VkBuffer testVertexbuffer;
    VkDeviceMemory testVertexBufferMemory;
    std::vector<uint32_t> testIndices;
    VkBuffer testIndexBuffer;
    VkDeviceMemory testIndexBufferMemory;

    VertexBufferManager()
    {
        testVertices = {
            Vertex{ {0, 0, 0}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
            Vertex{ {1, 0, 0}, {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
            Vertex{ {0, 1, 0}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} },
            Vertex{ {1, 1, 0}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f} }
        };
        testIndices = { 0, 1, 2, 1, 2, 3, 2, 1, 0, 3, 2, 1 };
    }

    void addChunkVertices(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    void createVertexBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer& buffer, VkDeviceMemory& memory, std::vector<Vertex>& vertices);
    void createIndexBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer& buffer, VkDeviceMemory& memory, std::vector<uint32_t>& indices);

    void cleanUpBuffers(VulkanCoreInfo* vulkanCoreInfo);

};

#include "Application.hpp"