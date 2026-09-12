#pragma once

#include "glm/fwd.hpp"
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <array>
#include <cinttypes>

struct Vertex {
    // pos 3 * 8
    // normal 3 * 8
    // uv 2 * 8
    // tex layer 32
    // in shadow 32 (is a 0 or 1 value)
    uint32_t posAndNorm;
    uint32_t normAndUv;
    float texCoord;
    uint32_t inShadow;
};

struct VertexLod {
    uint32_t pos; // A2B10G10R10
    uint32_t colorAndNormal; // A8B8G8R8
};

struct Vertex2D {
    alignas(16) glm::vec2 pos;
    alignas(16) glm::vec4 color;
    alignas(16) glm::vec2 texCoord;
    alignas(16) uint32_t texLayer;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex2D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex2D, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex2D, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex2D, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32_UINT;
        attributeDescriptions[3].offset = offsetof(Vertex2D, texLayer);

        return attributeDescriptions;
    }
};
