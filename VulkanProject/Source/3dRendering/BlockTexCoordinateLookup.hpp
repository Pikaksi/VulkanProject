#pragma once

#include "glm/vec2.hpp"

#include <vector>
#include <unordered_map>
#include <World/BlockType.hpp>

const int TEXTURE_BLOCK_VERTICAL_COUNT = 4;
const float BLOCK_TEX_SIZE = 1.0f / TEXTURE_BLOCK_VERTICAL_COUNT;
const float PIXEL_SIZE = 1.0f / 64.0f;


const glm::vec2 BLOCK_TEX_ADD_RIGHT = glm::vec2(BLOCK_TEX_SIZE - PIXEL_SIZE, 0.0f);
const glm::vec2 BLOCK_TEX_ADD_RIGHT_UP = glm::vec2(BLOCK_TEX_SIZE - PIXEL_SIZE, BLOCK_TEX_SIZE - PIXEL_SIZE);
const glm::vec2 BLOCK_TEX_ADD_UP = glm::vec2(0.0f, BLOCK_TEX_SIZE - PIXEL_SIZE);


const float BLOCK_TEX_SIZE_ACCURATE = 1.0f / TEXTURE_BLOCK_VERTICAL_COUNT;
const float TS = BLOCK_TEX_SIZE_ACCURATE;

const std::unordered_map<BlockType, std::vector<glm::vec2>> blockToTexCoordinate = {
	/*{ BlockType::dirt, std::vector<glm::vec2> {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 0.0f)
	} },*/
	{ BlockType::dirt, std::vector<glm::vec2> {
		glm::vec2(TS * 1 + PIXEL_SIZE / 2, 0.0f + PIXEL_SIZE / 2),
		glm::vec2(TS * 1 + PIXEL_SIZE / 2, 0.0f + PIXEL_SIZE / 2),
		glm::vec2(0.0f + PIXEL_SIZE / 2, 0.0f + PIXEL_SIZE / 2),
		glm::vec2(TS * 2 + PIXEL_SIZE / 2, 0.0f + PIXEL_SIZE / 2),
		glm::vec2(TS * 1 + PIXEL_SIZE / 2, 0.0f + PIXEL_SIZE / 2),
		glm::vec2(TS * 1 + PIXEL_SIZE / 2, 0.0f + PIXEL_SIZE / 2)
	} },
	{ BlockType::stone, std::vector<glm::vec2> {
		glm::vec2(TS * 2 + PIXEL_SIZE / 2, TS * 2 + PIXEL_SIZE / 2),
		glm::vec2(TS * 2 + PIXEL_SIZE / 2, TS * 2 + PIXEL_SIZE / 2),
		glm::vec2(TS * 2 + PIXEL_SIZE / 2, TS * 2 + PIXEL_SIZE / 2),
		glm::vec2(TS * 2 + PIXEL_SIZE / 2, TS * 2 + PIXEL_SIZE / 2),
		glm::vec2(TS * 2 + PIXEL_SIZE / 2, TS * 2 + PIXEL_SIZE / 2),
		glm::vec2(TS * 2 + PIXEL_SIZE / 2, TS * 2 + PIXEL_SIZE / 2)
	}, }
};