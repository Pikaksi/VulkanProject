#include "PlayerControls.hpp"

#include <X11/extensions/Xrender.h>
#include <ctime>
#include <iostream>

#include "BlockDataLookup.hpp"
#include "BlockType.hpp"
#include "CameraHandler.hpp"
#include "Chunk.hpp"
#include "PlaceBlock.hpp"
#include "PlayerInputHandler.hpp"
#include "blockEntityManager.hpp"
#include "assertm.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "priting.hpp"

struct RaycastResult
{
    bool isLookingAtBlock;
    bool isInsideBlock;
    float distance;
    glm::vec3 pos;
    glm::vec3 normal;
};

float timeUntilNextBlock(float pos, float vel)
{
    float velAbs = std::abs(velAbs);
    velAbs = std::max(velAbs, 0.0000001f);

    if (pos == std::round(pos)) {
        return 1.0f / velAbs;
    }
    if (vel > 0) {
        return std::abs(pos - std::ceil(pos)) / velAbs;
    }
    else {
        return std::abs(pos - std::floor(pos)) / velAbs;
    }
}

// Distance is wrong if rayVector is not normalized
bool rayIntersectsTriangle(const glm::vec3& rayOrigin,
                           const glm::vec3& rayVector,
                           const glm::vec3& triangleA,
                           const glm::vec3& triangleB,
                           const glm::vec3& triangleC,
                           float& distanceOut)
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    glm::vec3 edge1 = triangleB - triangleA;
    glm::vec3 edge2 = triangleC - triangleA;

    // Backface culling, assuming CCW-wound triangles.
    /*const glm::vec3 normal = glm::cross(edge1, edge2); // No need to normalize
    if (glm::dot(normal, rayVector) > 0)
        return false;*/

    glm::vec3 ray_cross_e2 = glm::cross(rayVector, edge2);
    float det = glm::dot(edge1, ray_cross_e2);

    if (abs(det) < epsilon)
        return false; // Ray is parallel to triangle

    float inv_det = 1.0 / det;
    glm::vec3 s = rayOrigin - triangleA;
    float u = inv_det * glm::dot(s, ray_cross_e2);

    if (u < -epsilon || u - 1 > epsilon)
        return false; // Ray passes outside edge2's bounds

    glm::vec3 s_cross_e1 = glm::cross(s, edge1);
    float v = inv_det * glm::dot(rayVector, s_cross_e1);

    if (v < -epsilon || u + v - 1 > epsilon)
        return false; // Ray passes outside edge1's bounds

    // The ray line intersects with the triangle.
    // We compute t to find where on the ray the intersection is.
    float distance = inv_det * glm::dot(edge2, s_cross_e1);

    if (distance > epsilon) // Ray intersection
    {
        distanceOut = distance;
        return true;
    }
    return false;
}

void addFaceOfTriangles(glm::vec3 pos, glm::vec3 pos2, glm::vec3 pos3, std::vector<glm::vec3>& triangles)
{
    glm::vec3 dir1 = -pos + pos2;
    glm::vec3 dir2 = -pos + pos3;
    triangles.push_back(pos);
    triangles.push_back(pos + dir1);
    triangles.push_back(pos + dir2);

    triangles.push_back(pos + dir1 + dir2);
    triangles.push_back(pos + dir2);
    triangles.push_back(pos + dir1);
}

RaycastResult raycastFindNextBlock(WorldManager& worldManager, glm::vec3 startPos, glm::vec3 lookDir, float range)
{
    lookDir = glm::normalize(lookDir);
    bool hasHitTriangle = false;
    float nearestDistance = 999.0f;
    glm::vec3 nearestNormal = {0, 0, 0};
    glm::i32vec3 blockLocation = round(startPos);

    std::vector<glm::vec3> triangles;
    addFaceOfTriangles({0, 0, 0}, {1, 0, 0}, {0, 1, 0}, triangles);
    addFaceOfTriangles({1, 0, 0}, {1, 0, 1}, {1, 1, 0}, triangles);
    addFaceOfTriangles({1, 0, 1}, {0, 0, 1}, {1, 1, 1}, triangles);
    addFaceOfTriangles({0, 0, 1}, {0, 0, 0}, {0, 1, 1}, triangles);
    addFaceOfTriangles({0, 0, 1}, {1, 0, 1}, {0, 0, 0}, triangles);
    addFaceOfTriangles({0, 1, 0}, {1, 1, 0}, {0, 1, 1}, triangles);

    const int cubeRange = (int)range + 1;
    for (int x = -cubeRange; x <= cubeRange; x++) {
        for (int y = -cubeRange; y <= cubeRange; y++) {
            for (int z = -cubeRange; z <= cubeRange; z++) {

                // TODO: use a better algorithm
                // Culling of blocks out of view
                float dotProd = glm::dot(glm::vec3{x, y, z} + glm::vec3{0.5f, 0.5f, 0.5f}, lookDir);
                if (dotProd < -2.0f) {
                    continue;
                }
                glm::vec3 projPos = dotProd * lookDir;
                if (glm::dot(-glm::vec3{x, y, z} + projPos, -glm::vec3{x, y, z} + projPos) > 4.0f) {
                    continue;
                }

                glm::i32vec3 loc = glm::i32vec3{x, y, z} + blockLocation;
                glm::vec3 pos = (glm::vec3)loc;
                glm::i32vec3 chunkLoc = worldToChunkLocation(loc);
                glm::i32vec3 blockLoc = worldToBlockLocation(loc);
                assertm(worldManager.chunks.contains(chunkLoc), "Chunk does not exist");
                BlockType block = chunkGetBlockAtLocation(blockLoc, worldManager.chunks.at(chunkLoc));

                if (block == BlockType::air) {
                    continue;
                }
                for (int i = 0; i < 12; i++) {
                    float distance = 0;
                    bool success = rayIntersectsTriangle(startPos,
                                                         lookDir,
                                                         triangles[i * 3] + pos,
                                                         triangles[i * 3 + 1] + pos,
                                                         triangles[i * 3 + 2] + pos,
                                                         distance);
                    if (success && distance < nearestDistance) {
                        /*std::cout << "  hit triangle. pos: \n"
                                  << "    " << (triangles[i * 3] + pos) << "\n"
                                  << "    " << (triangles[i * 3 + 1] + pos) << "\n"
                                  << "    " << (triangles[i * 3 + 2] + pos) << std::endl;*/
                        nearestDistance = distance;
                        nearestNormal = -glm::cross(-triangles[i * 3] + triangles[i * 3 + 1],
                                                    -triangles[i * 3] + triangles[i * 3 + 2]);
                        hasHitTriangle = true;
                    }
                }
            }
        }
    }

    if (!hasHitTriangle) {
        return RaycastResult{
            .isLookingAtBlock = false,
        };
    }
    else {
        glm::vec3 hitPos = startPos + nearestDistance * lookDir;
        return RaycastResult{
            .isLookingAtBlock = true,
            .isInsideBlock = round(startPos) == round(hitPos),
            .distance = nearestDistance,
            .pos = hitPos,
            .normal = nearestNormal,
        };
    }
}

void updatePlayerControls(glm::vec3 position,
                          WorldManager& worldManager,
                          BlockEntityManager& blockEntityManager,
                          ChunkRenderer& chunkRenderer,
                          CameraHandler& camerahandler,
                          PlayerInfo& playerInfo)
{
    if (inputHandler.mousePressed(GLFW_MOUSE_BUTTON_RIGHT) && !playerInfo.inventoryIsActive) {

        auto time1 = std::chrono::high_resolution_clock::now();
        RaycastResult result =
            raycastFindNextBlock(worldManager, camerahandler.position, camerahandler.cameraForwardDirection(), 6.0f);

        auto time2 = std::chrono::high_resolution_clock::now();
        auto timeSpan =
            std::chrono::duration<float, std::chrono::milliseconds::period>(time2 - time1).count();
        std::cout << "time: " << timeSpan << std::endl;

        std::cout << "Raycast Result:\n"
                  << "  hit block: " << result.isLookingAtBlock << "\n"
                  << "  pos: " << result.pos << "\n"
                  << "  loc: " << glm::i32vec3(result.pos + 0.1f * result.normal) << "\n"
                  << "  distance: " << result.distance << "\n"
                  << "  normal: " << result.normal << std::endl;

        if (!result.isLookingAtBlock)
            return;

        /*glm::ivec3 worldBlockLocation = floor(result.pos + 0.001f * result.normal);
        glm::ivec3 chunkLocation = worldToChunkLocation(worldBlockLocation);
        glm::ivec3 blockLocation = worldToBlockLocation(worldBlockLocation);

        Chunk& chunk = worldManager.chunks.at(chunkLocation);
        BlockType block = chunkGetBlockAtLocation(blockLocation.x, blockLocation.y, blockLocation.z, chunk);*/

        Item itemInHand = playerInfo.playerInventory.itemStacks[0].item;
        // TODO: Check for not replacing blocks
        if (itemInHand != Item::empty && itemToBlockType.contains(itemInHand)) {
            glm::ivec3 worldBlockLocation = floor(result.pos + 0.001f * result.normal);
            glm::ivec3 chunkLocation = worldToChunkLocation(worldBlockLocation);
            glm::ivec3 blockLocation = worldToBlockLocation(worldBlockLocation);

            BlockType blockToPlace = itemToBlockType.at(itemInHand);
            placeBlock(chunkLocation, blockLocation, blockToPlace, worldManager, blockEntityManager, chunkRenderer);

            playerInfo.interactedWithBlock = false;
            playerInfo.isLookingAtBlock = true;
            playerInfo.lookingAtLocationWorld = worldBlockLocation;
            playerInfo.lookingAtLocationChunk = chunkLocation;
            playerInfo.lookingAtLocationInChunk = blockLocation;
        }
        else {
            glm::ivec3 worldBlockLocation = floor(result.pos - 0.001f * result.normal);
            glm::ivec3 chunkLocation = worldToChunkLocation(worldBlockLocation);
            glm::ivec3 blockLocation = worldToBlockLocation(worldBlockLocation);

            playerInfo.interactedWithBlock = true;
            playerInfo.isLookingAtBlock = true;
            playerInfo.lookingAtLocationWorld = worldBlockLocation;
            playerInfo.lookingAtLocationChunk = chunkLocation;
            playerInfo.lookingAtLocationInChunk = blockLocation;
            std::cout << "interacted with block" << std::endl;
        }

    }
}
