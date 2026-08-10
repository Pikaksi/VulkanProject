#include "WorldVertexTracker.hpp"

#include "assertm.hpp"

void WorldVertexTracker::addLocation(VkDeviceSize memoryLocation, uint64_t dataCount, glm::ivec3 chunkLocation)
{
    WorldDrawCallData vertexBatchData(memoryLocation, dataCount, chunkLocation);

    trackedDrawCallData.push_back(vertexBatchData);
    drawCallDataLookup.insert(std::make_pair(memoryLocation, trackedDrawCallData.size() - 1));
}

void WorldVertexTracker::removeLocation(VkDeviceSize memoryLocation)
{
    VkDeviceSize newMemoryLocation = trackedDrawCallData.back().memoryLocation;

    uint64_t oldIndex = drawCallDataLookup.at(memoryLocation);
    if (trackedDrawCallData[oldIndex].memoryLocation != trackedDrawCallData.back().memoryLocation) {
        std::swap(trackedDrawCallData[oldIndex], trackedDrawCallData.back());

        assertm(drawCallDataLookup.contains(newMemoryLocation), "Removing tracked chunk failed");
        drawCallDataLookup.at(newMemoryLocation) = oldIndex;
    }
    trackedDrawCallData.pop_back();
    drawCallDataLookup.erase(memoryLocation);
}
