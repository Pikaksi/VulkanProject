#pragma once

#include <vector>

#include "Vertex.hpp"
#include "VulkanRendering/VulkanTypes.hpp"
#include "VulkanRendering/Buffers.hpp"
#include "Rendering/GPUMemoryBlock.hpp"

class VertexBufferManager
{

public:
    std::vector<uint32_t> vertexCounts;
    std::vector<VkBuffer> vertexBuffers;
    std::vector<VkDeviceMemory> vertexBufferMemories;

    std::vector<uint32_t> indexCounts;
    std::vector<VkBuffer> indexBuffers;
    std::vector<VkDeviceMemory> indexBufferMemories;

    // One buffer with a large amount of quad strip indices is created and looked at by all vertice lists which render quads only
    uint32_t quadStripIndexBufferQuadAmount = 500000;
    uint32_t quadStripIndexBufferIndexCount = 6 * quadStripIndexBufferQuadAmount;
    VkBuffer quadStripIndexBuffer;
    VkDeviceMemory quadStripIndexBufferMemory;

    GPUMemoryBlock worldGPUMemoryBlock;

    /*std::vector<Vertex> testVertices;
    VkBuffer testVertexbuffer;
    VkDeviceMemory testVertexBufferMemory;
    std::vector<uint32_t> testIndices;
    VkBuffer testIndexBuffer;
    VkDeviceMemory testIndexBufferMemory;*/

    VertexBufferManager() {}

    void addChunkVertices(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    void fillLargeQuadStripIndexBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool);
    void createGPUMemoryBlocks(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool);

    void cleanUpBuffers(VulkanCoreInfo* vulkanCoreInfo);

};

#include "Application.hpp"