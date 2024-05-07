#include <iostream>

#include "ChunkGenerator.hpp"

Chunk generateChunk(glm::i32vec3 chunkLocation)
{
	auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
	auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();

	fnFractal->SetSource(fnSimplex);
	fnFractal->SetOctaveCount(5);

	// The string is generated with the FastNoise2 noisetool.
	FastNoise::SmartNode<> fnGenerator = FastNoise::NewFromEncodedNodeTree("CAA=");

	std::vector<float> noiseOutput(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

	// Generate a CHUNK_SIZE x CHUNK_SIZE x CHUNK_SIZE area of noise
	fnGenerator->GenUniformGrid3D(noiseOutput.data(),
		chunkLocation.z * CHUNK_SIZE,
		chunkLocation.y * CHUNK_SIZE,
		chunkLocation.x * CHUNK_SIZE,
		CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE,
		frequency, 1337);
	int index = 0;

	Chunk chunk;
	glm::i32vec3 chunkBlockLocation = chunkLocation * 16;

	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {

				float noise = noiseOutput[index];
				index += 1;

				noise += heightNoiseMultiplier * (chunkLocation.y * CHUNK_SIZE + y);
				
				if (noise < 0.0f) {
					chunk.blocks[chunkLocationToIndex(x, y, z)] = stone;
				}
				else {
					chunk.blocks[chunkLocationToIndex(x, y, z)] = air;
				}
			}
		}
	}

	return chunk;
}

