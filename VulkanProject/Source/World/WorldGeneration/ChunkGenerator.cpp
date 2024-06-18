#include <iostream>

#include "ChunkGenerator.hpp"

const int grassDirtLayerHeight = 3;
const float airNoiseMinValue = 0.0f;

int noiseChunkLocationToIndex(const int x, const int y, const int z)
{
#ifndef NDEBUG
	if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= (CHUNK_SIZE + grassDirtLayerHeight) || z < 0 || z >= CHUNK_SIZE) {
		std::cout << "tried to access chunk out of range! x: " << x << " y: " << y << " z: " << z << "\n";
		throw std::runtime_error("tried to access chunk out of range!");
	}
#endif

	return z + y * CHUNK_SIZE + x * CHUNK_SIZE * (CHUNK_SIZE + grassDirtLayerHeight);
}

Chunk generateChunk(glm::i32vec3 chunkLocation)
{
	// The string is generated with the FastNoise2 noisetool.
	FastNoise::SmartNode<> fnGenerator = FastNoise::NewFromEncodedNodeTree("GQANAAQAAAAAAABACAAAAAAAPwAAAAAAAQQAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=");

	std::vector<float> noiseOutput(CHUNK_SIZE * (CHUNK_SIZE + grassDirtLayerHeight) * CHUNK_SIZE);

	fnGenerator->GenUniformGrid3D(noiseOutput.data(),
		chunkLocation.z * CHUNK_SIZE,
		chunkLocation.y * CHUNK_SIZE,
		chunkLocation.x * CHUNK_SIZE,
		CHUNK_SIZE, CHUNK_SIZE + grassDirtLayerHeight, CHUNK_SIZE,
		frequency, 1337);

	Chunk chunk(true);

	for (int x = 0; x < CHUNK_SIZE; x++) {

		for (int y = 0; y < CHUNK_SIZE; y++) {

			for (int z = 0; z < CHUNK_SIZE; z++) {

				float noise = noiseOutput[noiseChunkLocationToIndex(x, y, z)];

				if (noise >= airNoiseMinValue) {
					chunk.blocks[chunkLocationToIndex(x, y, z)] = BlockType::air;
					continue;
				}

				bool blockIsStillAir = true;
				for (int yExtra = 1; yExtra < grassDirtLayerHeight + 1; yExtra++) {
					if (noiseOutput[noiseChunkLocationToIndex(x, y + yExtra, z)] >= airNoiseMinValue) {
						chunk.blocks[chunkLocationToIndex(x, y, z)] = yExtra == 1 ? BlockType::grass : BlockType::dirt;
						blockIsStillAir = false;
						break;
					}
				}

				if (blockIsStillAir) {
					chunk.blocks[chunkLocationToIndex(x, y, z)] = BlockType::stone;
				}
			}
		}
	}
	
	bool containsDifferentBlocks = false;
	BlockType previousBlockType = chunk.blocks[0];

	for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
		if (previousBlockType != chunk.blocks[i]) {
			containsDifferentBlocks = true;
			break;
		}
	}
	if (!containsDifferentBlocks) {
		chunk = Chunk(false); // contains only one element in block array to reduce space by CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE amount.
		chunk.blocks[0] = previousBlockType;
	}
	return chunk;
}

