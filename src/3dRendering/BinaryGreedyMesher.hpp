#pragma once

#include <glm/vec3.hpp>

#include <vector>

#include "Rendering/Vertex.hpp"
#include "World/WorldManager.hpp"

void binaryGreedyMeshChunk(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices);
