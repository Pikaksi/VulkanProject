#include <glm/vec3.hpp>

#include <vector>

#include "World/Chunk.hpp"
#include "Vertex.hpp"
#include "World/WorldManager.hpp"

void binaryGreedyMeshChunk(WorldManager worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices);