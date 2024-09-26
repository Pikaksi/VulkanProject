#include "WorldVertexTracker.hpp"


void WorldVertexTracker::addLocation(VkDeviceSize memoryLocation, uint32_t dataCount, glm::vec3 chunkLocation)
{
    WorldDrawCallData vertexBatchData(memoryLocation, dataCount, chunkLocation);

    trackedDrawCallData.push_back(vertexBatchData);
    DrawCallDataLookup.insert(std::make_pair(memoryLocation, trackedDrawCallData.size() - 1));
}

void WorldVertexTracker::removeLocation(VkDeviceSize memoryLocation)
{
    uint32_t index = DrawCallDataLookup.at(memoryLocation);
    VkDeviceSize backMemoryLocation = trackedDrawCallData.back().memoryLocation;

    DrawCallDataLookup.erase(memoryLocation);
    DrawCallDataLookup.at(backMemoryLocation) = index;

    std::swap(trackedDrawCallData[index], trackedDrawCallData.back());
    trackedDrawCallData.pop_back();
}
