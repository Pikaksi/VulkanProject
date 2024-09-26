#pragma once

#include <vector>
#include <unordered_map>

#include "glm/vec3.hpp"
#include "vulkan/vulkan.h"

struct WorldDrawCallData
{
    VkDeviceSize memoryLocation;
    uint32_t dataCount;
    glm::vec3 chunkLocation;
};

class WorldVertexTracker
{
public:
    void removeLocation(VkDeviceSize memoryLocation);
    void addLocation(VkDeviceSize memoryLocation, uint32_t dataCount, glm::vec3 chunkLocation);
    std::vector<WorldDrawCallData>& getData() { return trackedDrawCallData; }

private:
    std::vector<WorldDrawCallData> trackedDrawCallData;
    std::unordered_map<VkDeviceSize, uint32_t> DrawCallDataLookup;
};