#include "WorldVertexTracker.hpp"

#include <iostream>
#include <stdexcept>

void WorldVertexTracker::addLocation(VkDeviceSize memoryLocation, uint32_t dataCount, glm::ivec3 chunkLocation)
{
    WorldDrawCallData vertexBatchData(memoryLocation, dataCount, chunkLocation);

    trackedDrawCallData.push_back(vertexBatchData);
    drawCallDataLookup.insert(std::make_pair(memoryLocation, trackedDrawCallData.size() - 1));
    std::cout << "Added: " << memoryLocation << " New size = " << trackedDrawCallData.size() << "\n";
}

void WorldVertexTracker::removeLocation(VkDeviceSize memoryLocation)
{
    VkDeviceSize newMemoryLocation = trackedDrawCallData.back().memoryLocation;

    uint32_t oldIndex = drawCallDataLookup.at(memoryLocation);
    if (trackedDrawCallData[oldIndex].memoryLocation != trackedDrawCallData.back().memoryLocation) {
        std::swap(trackedDrawCallData[oldIndex], trackedDrawCallData.back());

#ifndef NDEBUG
        if (!drawCallDataLookup.contains(newMemoryLocation)) {
            throw std::runtime_error("Removing tracked chunk failed");
        }
#endif
        drawCallDataLookup.at(newMemoryLocation) = oldIndex;
    }
    trackedDrawCallData.pop_back();
    drawCallDataLookup.erase(memoryLocation);
}
