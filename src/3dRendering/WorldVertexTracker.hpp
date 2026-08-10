#pragma once

#include <vector>
#include <unordered_map>

#include "glm/vec3.hpp"
#include "vulkan/vulkan.h"

struct WorldDrawCallData
{
    VkDeviceSize memoryLocation;
    uint32_t dataCount;
    glm::ivec3 chunkLocation;
    bool lod;
};

class WorldVertexTracker
{
public:
    void removeLocation(VkDeviceSize memoryLocation);
    void addLocation(VkDeviceSize memoryLocation, uint64_t dataCount, glm::ivec3 chunkLocation, int lod);

    std::vector<WorldDrawCallData> trackedDrawCallData;
    std::unordered_map<VkDeviceSize, uint64_t> drawCallDataLookup;
};
