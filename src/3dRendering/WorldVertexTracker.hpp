#pragma once

#include <vector>
#include <unordered_map>

#include "glm/vec3.hpp"
#include "vulkan/vulkan.h"

struct WorldDrawCallData
{
    VkDeviceSize memoryLocation;
    uint64_t dataSize;
    glm::ivec3 chunkLocation;
    int lod;
    bool fullDetail;
};

class WorldVertexTracker
{
public:
    void removeLocation(VkDeviceSize memoryLocation);
    void addLocation(VkDeviceSize memoryLocation, uint64_t dataSize, glm::ivec3 chunkLocation, int lod, bool fullDetail);

    std::vector<WorldDrawCallData> trackedDrawCallData;
    std::unordered_map<VkDeviceSize, uint64_t> drawCallDataLookup;
};
