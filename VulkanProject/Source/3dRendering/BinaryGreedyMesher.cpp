#include "BinaryGreedyMesher.hpp"

#include <numbers>
#include <bitset>

#include "BlockTexCoordinateLookup.hpp"
#include "Constants.hpp"
#include "World/BlockDataLookup.hpp"
#include "Rendering/TextureCreator.hpp"

typedef uint64_t BlockBitMask;

void xDirectionMergeFaces(int y, int z, Chunk* chunk, BlockBitMask* rxBlockFaceBitMask, std::vector<Vertex>& vertices, glm::i32vec3 vertexOffset, bool directionIsPositive);
void yDirectionMergeFaces(int x, int z, Chunk* chunk, BlockBitMask* rxBlockFaceBitMask, std::vector<Vertex>& vertices, glm::i32vec3 vertexOffset, bool directionIsPositive);

const uint64_t debruijn64 = 0x03f79d71b4cb0a89;
const int index64[64] = {
	0, 1, 48, 2, 57, 49, 28, 3,
	61, 58, 50, 42, 38, 29, 17, 4,
	62, 55, 59, 36, 53, 51, 43, 22,
	45, 39, 33, 30, 24, 18, 12, 5,
	63, 47, 56, 27, 60, 41, 37, 16,
	54, 35, 52, 21, 44, 32, 23, 11,
	46, 26, 40, 15, 34, 20, 31, 10,
	25, 14, 19, 9, 13, 8, 7, 6
};

// https://www.chessprogramming.org/index.php?title=BitScan
int LovestSignificantBitIndex(uint64_t bitboard)
{
	return index64[((bitboard & (0 - bitboard)) * debruijn64) >> 58];
}

void printBitMask(BlockBitMask blockBitMask)
{
	for (int i = CHUNK_SIZE + 1; i >= 0; i--) {
		std::cout << (uint64_t)((blockBitMask & (1 << i)) >> i);
	}
}

void printChunkLayerX(Chunk& chunk, int x)
{
	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			std::cout << (int)chunkGetBlockAtLocation(x, y, z, &chunk);
		}
		std::cout << "\n";
	}
}

int chunk2dLocationToIndex(const int x, const int y)
{
#ifndef NDEBUG
	if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE) {
		throw std::runtime_error("tried to access blockFaceBitMask out of range!");
	}
#endif

	return x + y * CHUNK_SIZE;
}

constexpr BlockBitMask chunkOuterBitsOff()
{
	return ~(((BlockBitMask)0b1) | (((BlockBitMask)0b1) << (CHUNK_SIZE + 1)));
}

BlockBitMask getBlockFacesLeftShift(BlockBitMask blockBitMask)
{
	return (((~blockBitMask) >> 1) & blockBitMask) & chunkOuterBitsOff();
}

BlockBitMask getBlockFacesRightShift(BlockBitMask blockBitMask)
{
	return (((~blockBitMask) << 1) & blockBitMask) & chunkOuterBitsOff();
}

void binaryGreedyMeshChunk(WorldManager worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices)
{
	Chunk* chunk = &worldManager.chunks[chunkLocation];

	if (!chunk->containsDifferentBlocks) {
		return;
	}

	Chunk* chunkPX = nullptr;
	Chunk* chunkNX = nullptr;
	Chunk* chunkPY = nullptr;
	Chunk* chunkNY = nullptr;
	Chunk* chunkPZ = nullptr;
	Chunk* chunkNZ = nullptr;

	glm::i32vec3 pX = glm::i32vec3(chunkLocation.x + 1, chunkLocation.y, chunkLocation.z);
	glm::i32vec3 nX = glm::i32vec3(chunkLocation.x - 1, chunkLocation.y, chunkLocation.z);
	glm::i32vec3 pY = glm::i32vec3(chunkLocation.x, chunkLocation.y + 1, chunkLocation.z);
	glm::i32vec3 nY = glm::i32vec3(chunkLocation.x, chunkLocation.y - 1, chunkLocation.z);
	glm::i32vec3 pZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z + 1);
	glm::i32vec3 nZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z - 1);

	if (worldManager.chunks.contains(pX)) {
		chunkPX = &worldManager.chunks[pX];
	}
	if (worldManager.chunks.contains(nX)) {
		chunkNX = &worldManager.chunks[nX];
	}
	if (worldManager.chunks.contains(pY)) {
		chunkPY = &worldManager.chunks[pY];
	}
	if (worldManager.chunks.contains(nY)) {
		chunkNY = &worldManager.chunks[nY];
	}
	if (worldManager.chunks.contains(pZ)) {
		chunkPZ = &worldManager.chunks[pZ];
	}
	if (worldManager.chunks.contains(nZ)) {
		chunkNZ = &worldManager.chunks[nZ];
	}

	// rx = right x coordinate
	//std::cout << "chunk\n";
	BlockBitMask* rxBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
	BlockBitMask* lxBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {

			BlockBitMask blockBitMask = 0;
			if (chunkNX != nullptr) {
				blockBitMask = (BlockBitMask)isSolidBlock[chunkGetBlockAtLocation(CHUNK_SIZE - 1, y, z, chunkNX)];
			}
			
			for (int x = 0; x < CHUNK_SIZE; x++) {
				blockBitMask |= (BlockBitMask)isSolidBlock[chunkGetBlockAtLocation(x, y, z, chunk)] << (x + 1);
			}
			if (chunkPX != nullptr) {
				blockBitMask |= (BlockBitMask)isSolidBlock[chunkGetBlockAtLocation(0, y, z, chunkPX)] << CHUNK_SIZE + 1;
			}
			rxBlockFaceBitMask[chunk2dLocationToIndex(y, z)] = getBlockFacesLeftShift(blockBitMask);
			lxBlockFaceBitMask[chunk2dLocationToIndex(y, z)] = getBlockFacesRightShift(blockBitMask);
		}
	}

	BlockBitMask* ryBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
	BlockBitMask* lyBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {

			BlockBitMask blockBitMask = 0;
			if (chunkNX != nullptr) {
				blockBitMask = (BlockBitMask)isSolidBlock[chunkGetBlockAtLocation(x, CHUNK_SIZE - 1, z, chunkNX)];
			}
			for (int y = 0; y < CHUNK_SIZE; y++) {
				blockBitMask |= (BlockBitMask)isSolidBlock[chunk->blocks[chunkLocationToIndex(x, y, z)]] << (y + 1);
			}
			if (chunkPX != nullptr) {
				blockBitMask |= (BlockBitMask)isSolidBlock[chunkGetBlockAtLocation(x, 0, z, chunkPX)] << CHUNK_SIZE + 1;
			}
			ryBlockFaceBitMask[chunk2dLocationToIndex(x, z)] = getBlockFacesLeftShift(blockBitMask);
			lyBlockFaceBitMask[chunk2dLocationToIndex(x, z)] = getBlockFacesRightShift(blockBitMask);
		}
	}

	BlockBitMask* rzBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
	BlockBitMask* lzBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {

			BlockBitMask blockBitMask = 0;

			if (chunkNX != nullptr) {
				blockBitMask = (BlockBitMask)isSolidBlock[chunkGetBlockAtLocation(x, y, CHUNK_SIZE - 1, chunkNX)];
			}
			for (int z = 0; z < CHUNK_SIZE; z++) {
				blockBitMask |= (BlockBitMask)isSolidBlock[chunk->blocks[chunkLocationToIndex(x, y, z)]] << (z + 1);
			}
			if (chunkPX != nullptr) {
				blockBitMask |= (BlockBitMask)isSolidBlock[chunkGetBlockAtLocation(x, y, 0, chunkPX)] << CHUNK_SIZE + 1;
			}
			rzBlockFaceBitMask[chunk2dLocationToIndex(x, y)] = getBlockFacesLeftShift(blockBitMask);
			lzBlockFaceBitMask[chunk2dLocationToIndex(x, y)] = getBlockFacesRightShift(blockBitMask);
		}
	}
	/*std::cout << "face rx\n";
	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			std::cout << "y = " << y << " z = " << z << " ";
			printBitMask(rxBlockFaceBitMask[y][z]);
			std::cout << "\n";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
	for (int x = 0; x < CHUNK_SIZE; x++) {
		std::cout << "chunk layer at x: " << x << "\n";
		printChunkLayerX(chunk, x);
	}
	std::cout << "\n";*/

	glm::i32vec3 chunkBlockLocationOffset = chunkLocation * CHUNK_SIZE;

	// todo: test if seperate loops is faster because of cache locality
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int k = 0; k < CHUNK_SIZE; k++) {

			xDirectionMergeFaces(i, k, chunk, rxBlockFaceBitMask, vertices, chunkBlockLocationOffset, true);
			xDirectionMergeFaces(i, k, chunk, lxBlockFaceBitMask, vertices, chunkBlockLocationOffset, false);

			yDirectionMergeFaces(i, k, chunk, ryBlockFaceBitMask, vertices, chunkBlockLocationOffset, true);
			yDirectionMergeFaces(i, k, chunk, lyBlockFaceBitMask, vertices, chunkBlockLocationOffset, false);
		}
	}
}

inline void addVerticesRight(BlockType blockType, glm::i32vec3 blockLocation, int lenght, int height, std::vector<Vertex>& vertices)
{
	float textureArrayIndex = blockTypeToTexLayer.at(blockType)[0];
	vertices.push_back(Vertex{ {blockLocation.x + 1, blockLocation.y,		   blockLocation.z		   }, {0.9f, 0.9f, 0.9f}, {0.0f + UV_EDGE_CORRECTION,		   1.0f * height - UV_EDGE_CORRECTION}, textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x + 1, blockLocation.y,		   blockLocation.z + lenght}, {0.9f, 0.9f, 0.9f}, {1.0f * lenght - UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION}, textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x + 1, blockLocation.y + height, blockLocation.z + lenght}, {0.9f, 0.9f, 0.9f}, {1.0f * lenght - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION},		    textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x + 1, blockLocation.y + height, blockLocation.z		   }, {0.9f, 0.9f, 0.9f}, {0.0f + UV_EDGE_CORRECTION,		   0.0f + UV_EDGE_CORRECTION},		    textureArrayIndex });
}

inline void addVerticesLeft(BlockType blockType, glm::i32vec3 blockLocation, int lenght, int height, std::vector<Vertex>& vertices)
{
	float textureArrayIndex = blockTypeToTexLayer.at(blockType)[1];
	vertices.push_back(Vertex{ {blockLocation.x, blockLocation.y,		   blockLocation.z		   }, {0.7f, 0.7f, 0.7f}, {0.0f + UV_EDGE_CORRECTION,		   1.0f * height - UV_EDGE_CORRECTION}, textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x, blockLocation.y + height, blockLocation.z		   }, {0.7f, 0.7f, 0.7f}, {0.0f + UV_EDGE_CORRECTION,		   0.0f + UV_EDGE_CORRECTION},		    textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x, blockLocation.y + height, blockLocation.z + lenght}, {0.7f, 0.7f, 0.7f}, {1.0f * lenght - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION},		    textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x, blockLocation.y,		   blockLocation.z + lenght}, {0.7f, 0.7f, 0.7f}, {1.0f * lenght - UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION}, textureArrayIndex });
}

inline void addVerticesUp(BlockType blockType, glm::i32vec3 blockLocation, int width, int lenght, std::vector<Vertex>& vertices)
{
	float textureArrayIndex = blockTypeToTexLayer.at(blockType)[2];
	vertices.push_back(Vertex{ {blockLocation.x,         blockLocation.y + 1, blockLocation.z		  }, {0.9f, 0.9f, 0.9f}, {0.0f + UV_EDGE_CORRECTION,		 1.0f * lenght - UV_EDGE_CORRECTION}, textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x + width, blockLocation.y + 1, blockLocation.z		  }, {0.9f, 0.9f, 0.9f}, {1.0f * width - UV_EDGE_CORRECTION, 1.0f * lenght - UV_EDGE_CORRECTION}, textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x + width, blockLocation.y + 1, blockLocation.z + lenght}, {0.9f, 0.9f, 0.9f}, {1.0f * width - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION},	      textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x,         blockLocation.y + 1, blockLocation.z + lenght}, {0.9f, 0.9f, 0.9f}, {0.0f + UV_EDGE_CORRECTION,		 0.0f + UV_EDGE_CORRECTION},	      textureArrayIndex });
}

inline void addVerticesDown(BlockType blockType, glm::i32vec3 blockLocation, int width, int lenght, std::vector<Vertex>& vertices)
{
	float textureArrayIndex = blockTypeToTexLayer.at(blockType)[3];
	vertices.push_back(Vertex{ {blockLocation.x,         blockLocation.y, blockLocation.z		  }, {0.9f, 0.9f, 0.9f}, {0.0f + UV_EDGE_CORRECTION,		 1.0f * lenght - UV_EDGE_CORRECTION}, textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x,         blockLocation.y, blockLocation.z + lenght}, {0.9f, 0.9f, 0.9f}, {0.0f + UV_EDGE_CORRECTION,		 0.0f + UV_EDGE_CORRECTION},	      textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x + width, blockLocation.y, blockLocation.z + lenght}, {0.9f, 0.9f, 0.9f}, {1.0f * width - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION},	      textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x + width, blockLocation.y, blockLocation.z		  }, {0.9f, 0.9f, 0.9f}, {1.0f * width - UV_EDGE_CORRECTION, 1.0f * lenght - UV_EDGE_CORRECTION}, textureArrayIndex });
}

inline void addVerticesForward(BlockType blockType, glm::i32vec3 blockLocation, int width, int height, std::vector<Vertex>& vertices)
{
	float textureArrayIndex = blockTypeToTexLayer.at(blockType)[4];
	vertices.push_back(Vertex{ {blockLocation.x,         blockLocation.y,		   blockLocation.z + 1}, {0.9f, 0.9f, 0.9f}, {0.0f + UV_EDGE_CORRECTION,		 1.0f * height - UV_EDGE_CORRECTION}, textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x + width, blockLocation.y,		   blockLocation.z + 1}, {0.9f, 0.9f, 0.9f}, {1.0f * width - UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION}, textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x + width, blockLocation.y + height, blockLocation.z + 1}, {0.9f, 0.9f, 0.9f}, {1.0f * width - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION},		  textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x,         blockLocation.y + height, blockLocation.z + 1}, {0.9f, 0.9f, 0.9f}, {0.0f + UV_EDGE_CORRECTION,		 0.0f + UV_EDGE_CORRECTION},		  textureArrayIndex });
}

inline void addVerticesBackward(BlockType blockType, glm::i32vec3 blockLocation, int width, int height, std::vector<Vertex>& vertices)
{
	float textureArrayIndex = blockTypeToTexLayer.at(blockType)[5];
	vertices.push_back(Vertex{ {blockLocation.x,         blockLocation.y,		   blockLocation.z}, {0.9f, 0.9f, 0.9f}, {0.0f + UV_EDGE_CORRECTION,		 1.0f * height - UV_EDGE_CORRECTION}, textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x + width, blockLocation.y,		   blockLocation.z}, {0.9f, 0.9f, 0.9f}, {1.0f * width - UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION}, textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x + width, blockLocation.y + height, blockLocation.z}, {0.9f, 0.9f, 0.9f}, {1.0f * width - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION},		  textureArrayIndex });
	vertices.push_back(Vertex{ {blockLocation.x,         blockLocation.y + height, blockLocation.z}, {0.9f, 0.9f, 0.9f}, {0.0f + UV_EDGE_CORRECTION,		 0.0f + UV_EDGE_CORRECTION},		  textureArrayIndex });
}

void xDirectionMergeFaces(int y, int z, Chunk* chunk, BlockBitMask* rxBlockFaceBitMask, std::vector<Vertex>& vertices, glm::i32vec3 vertexOffset, bool directionIsPositive)
{
	BlockBitMask blockBitMask = rxBlockFaceBitMask[chunk2dLocationToIndex(y, z)];
	while (blockBitMask != 0)
	{
		int height = 1;
		int lenght = 1;
		BlockBitMask startingBlock = blockBitMask & (0 - blockBitMask); // get least significant bit

		// The location takes into account that the neighboring chunk block is at index 0.
		// The first block in this chunk has the value x = 1.
		int x = LovestSignificantBitIndex(startingBlock);
		BlockType originalBlockType = chunk->blocks[chunkLocationToIndex(x - 1, y, z)];

		for (int yExpansion = 1; yExpansion < CHUNK_SIZE - y; yExpansion++) {

			if (((blockBitMask & rxBlockFaceBitMask[chunk2dLocationToIndex(y + yExpansion, z)]) == 0)
				|| (originalBlockType != chunk->blocks[chunkLocationToIndex(x - 1, y + yExpansion, z)])) {
				break;
			}
			// Turn off blocks that can be merged.
			rxBlockFaceBitMask[chunk2dLocationToIndex(y + yExpansion, z)] ^= startingBlock;
			height++;
		}

		for (int zExpansion = 1; zExpansion < CHUNK_SIZE - z; zExpansion++) {
			bool cantExpand = false;
			for (int yExpansion = 0; yExpansion < height; yExpansion++) {
				if ((blockBitMask & rxBlockFaceBitMask[chunk2dLocationToIndex(y + yExpansion, z + zExpansion)]) == 0
					|| originalBlockType != chunk->blocks[chunkLocationToIndex(x - 1, y + yExpansion, z + zExpansion)])
				{
					cantExpand = true;
					break;
				}
			}
			if (cantExpand) {
				break;
			}
			// Turn off blocks that can be merged.
			for (int yExpansion = 0; yExpansion < height; yExpansion++) {
				rxBlockFaceBitMask[chunk2dLocationToIndex(y + yExpansion, z + zExpansion)] ^= startingBlock;
			}
			lenght++;
		}

		blockBitMask ^= startingBlock; // turn off the bit that was used

		if (directionIsPositive) {
			addVerticesRight(originalBlockType, glm::i32vec3(x - 1, y, z) + vertexOffset, lenght, height, vertices);
		}
		else {
			addVerticesLeft(originalBlockType, glm::i32vec3(x - 1, y, z) + vertexOffset, lenght, height, vertices);
		}
	}
}

void yDirectionMergeFaces(int x, int z, Chunk* chunk, BlockBitMask* rxBlockFaceBitMask, std::vector<Vertex>& vertices, glm::i32vec3 vertexOffset, bool directionIsPositive)
{
	BlockBitMask blockBitMask = rxBlockFaceBitMask[chunk2dLocationToIndex(x, z)];
	while (blockBitMask != 0)
	{
		int width = 1;
		int lenght = 1;
		BlockBitMask startingBlock = blockBitMask & (0 - blockBitMask); // get least significant bit

		// The location takes into account that the neighboring chunk block is at index 0.
		// The first block in this chunk has the value x = 1.
		int y = LovestSignificantBitIndex(startingBlock);
		BlockType originalBlockType = chunk->blocks[chunkLocationToIndex(x, y - 1, z)];

		for (int xExpansion = 1; xExpansion < CHUNK_SIZE - x; xExpansion++) {

			if (((blockBitMask & rxBlockFaceBitMask[chunk2dLocationToIndex(x + xExpansion, z)]) == 0)
				|| (originalBlockType != chunk->blocks[chunkLocationToIndex(x + xExpansion, y - 1, z)])) {
				break;
			}
			// Turn off blocks that can be merged.
			rxBlockFaceBitMask[chunk2dLocationToIndex(x + xExpansion, z)] ^= startingBlock;
			width++;
		}

		for (int zExpansion = 1; zExpansion < CHUNK_SIZE - z; zExpansion++) {
			bool cantExpand = false;
			for (int xExpansion = 0; xExpansion < width; xExpansion++) {
				if ((blockBitMask & rxBlockFaceBitMask[chunk2dLocationToIndex(x + xExpansion, z + zExpansion)]) == 0
					|| originalBlockType != chunk->blocks[chunkLocationToIndex(x + xExpansion, y - 1, z + zExpansion)])
				{
					cantExpand = true;
					break;
				}
			}
			if (cantExpand) {
				break;
			}
			// Turn off blocks that can be merged.
			for (int yExpansion = 0; yExpansion < width; yExpansion++) {
				rxBlockFaceBitMask[chunk2dLocationToIndex(x + yExpansion, z + zExpansion)] ^= startingBlock;
			}
			lenght++;
		}

		blockBitMask ^= startingBlock; // turn off the bit that was used

		if (directionIsPositive) {
			addVerticesUp(originalBlockType, glm::i32vec3(x, y - 1, z) + vertexOffset, width, lenght, vertices);
		}
		else {
			addVerticesDown(originalBlockType, glm::i32vec3(x, y - 1, z) + vertexOffset, width, lenght, vertices);
		}
	}
}