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
	auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
	auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();

	fnFractal->SetSource(fnSimplex);
	fnFractal->SetOctaveCount(5);

	// The string is generated with the FastNoise2 noisetool.
	FastNoise::SmartNode<> fnGenerator = FastNoise::NewFromEncodedNodeTree("CAA=");

	std::vector<float> noiseOutput(CHUNK_SIZE * (CHUNK_SIZE + grassDirtLayerHeight) * CHUNK_SIZE);

	fnGenerator->GenUniformGrid3D(noiseOutput.data(),
		chunkLocation.z * CHUNK_SIZE,
		chunkLocation.y * CHUNK_SIZE,
		chunkLocation.x * CHUNK_SIZE,
		CHUNK_SIZE, CHUNK_SIZE + grassDirtLayerHeight, CHUNK_SIZE,
		frequency, 1337);

	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE + grassDirtLayerHeight; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {

				int xLocation = (chunkLocation.x * CHUNK_SIZE + x);
				int zLocation = (chunkLocation.z * CHUNK_SIZE + z);

				float xValue = std::pow(1 / (1 + std::pow(100, -std::abs(0.02f * xLocation))) * 2 - 1, 8);
				float zValue = std::pow(1 / (1 + std::pow(100, -std::abs(0.02f * zLocation))) * 2 - 1, 8);

				noiseOutput[noiseChunkLocationToIndex(x, y, z)] += std::max(xValue, zValue) * heightNoiseMultiplier * (chunkLocation.y * CHUNK_SIZE + y);
			}
		}
	}

	Chunk chunk;
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

	return chunk;
}

