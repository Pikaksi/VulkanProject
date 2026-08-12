#pragma once

#include "glm/vec3.hpp"

struct Vec3LocalizedHash {
    std::size_t operator()(const glm::ivec3& p) const {
        // Keeps only the lowest 8 bits (0 to 255) of each coordinate
        // uint32_t packed = (p.x & 0xFF) | ((p.y & 0xFF) << 8) | ((p.z & 0xFF) << 16);

        // This is safe as long as the active loaded area is smaller than 1024x1024x1024
        uint32_t packed = (p.x & 0x3FF) | ((p.y & 0x3FF) << 10) | ((p.z & 0x3FF) << 20);
        return packed; 
    }
};
