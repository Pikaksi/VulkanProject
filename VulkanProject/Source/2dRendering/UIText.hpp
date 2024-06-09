#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "UIQuad.hpp"

class UIText
{
private:
	float x;
	float y;
	float letterHeight;
	std::vector<UIQuad> textQuads;

public:
	UIText() {}
	UIText(float x, float y, float letterHeight, std::string text)
	{
		this->x = x;
		this->y = y;
		this->letterHeight = letterHeight;
		setText(text);
	}

	void setText(std::string text);
	void addMeshData(int screenWidth, int screenHeight, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
};

const float LETTERS_IN_ROW = 16.0f;
const float LETTERS_IN_COLUMN = 8.0f;

const float LETTER_TEX_WIDTH = 1.0f / LETTERS_IN_ROW;
const float LETTER_TEX_HEIGHT = 1.0f / LETTERS_IN_COLUMN;


const std::unordered_map<char, std::pair<float, float>> charToTexLocation = std::unordered_map<char, std::pair<float, float>>{
	{ '0', std::pair<float, float>(0.0f,         0.0f) },
	{ '1', std::pair<float, float>(1.0f / LETTERS_IN_ROW, 0.0f) },
	{ '2', std::pair<float, float>(2.0f / LETTERS_IN_ROW, 0.0f) },
	{ '3', std::pair<float, float>(3.0f / LETTERS_IN_ROW, 0.0f) },
	{ '4', std::pair<float, float>(4.0f / LETTERS_IN_ROW, 0.0f) },
	{ '5', std::pair<float, float>(5.0f / LETTERS_IN_ROW, 0.0f) },
	{ '6', std::pair<float, float>(6.0f / LETTERS_IN_ROW, 0.0f) },
	{ '7', std::pair<float, float>(7.0f / LETTERS_IN_ROW, 0.0f) },
	{ '8', std::pair<float, float>(8.0f / LETTERS_IN_ROW, 0.0f) },
	{ '9', std::pair<float, float>(9.0f / LETTERS_IN_ROW, 0.0f) },
	{ 'a', std::pair<float, float>(10.0f / LETTERS_IN_ROW, 0.0f) },
	{ 'b', std::pair<float, float>(11.0f / LETTERS_IN_ROW, 0.0f) },
	{ 'c', std::pair<float, float>(12.0f / LETTERS_IN_ROW, 0.0f) },
	{ 'd', std::pair<float, float>(13.0f / LETTERS_IN_ROW, 0.0f) },
	{ 'e', std::pair<float, float>(14.0f / LETTERS_IN_ROW, 0.0f) },
	{ 'f', std::pair<float, float>(15.0f / LETTERS_IN_ROW, 0.0f) },

	{ 'g', std::pair<float, float>(0.0f,         1.0f / 8.0f) },
	{ 'h', std::pair<float, float>(1.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'i', std::pair<float, float>(2.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'j', std::pair<float, float>(3.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'k', std::pair<float, float>(4.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'l', std::pair<float, float>(5.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'm', std::pair<float, float>(6.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'n', std::pair<float, float>(7.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'o', std::pair<float, float>(8.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'p', std::pair<float, float>(9.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'q', std::pair<float, float>(10.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'r', std::pair<float, float>(11.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 's', std::pair<float, float>(12.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 't', std::pair<float, float>(13.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'u', std::pair<float, float>(14.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },
	{ 'v', std::pair<float, float>(15.0f / LETTERS_IN_ROW, 1.0f / LETTERS_IN_COLUMN) },

	{ 'w', std::pair<float, float>(0.0f,         2.0f / 8.0f) },
	{ 'x', std::pair<float, float>(1.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'y', std::pair<float, float>(2.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'z', std::pair<float, float>(3.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'å', std::pair<float, float>(4.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'ä', std::pair<float, float>(5.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'ö', std::pair<float, float>(6.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'A', std::pair<float, float>(7.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'B', std::pair<float, float>(8.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'C', std::pair<float, float>(9.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'D', std::pair<float, float>(10.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'E', std::pair<float, float>(11.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'F', std::pair<float, float>(12.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'G', std::pair<float, float>(13.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'H', std::pair<float, float>(14.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },
	{ 'I', std::pair<float, float>(15.0f / LETTERS_IN_ROW, 2.0f / LETTERS_IN_COLUMN) },

	{ 'J', std::pair<float, float>(0.0f,         3.0f / 8.0f) },
	{ 'K', std::pair<float, float>(1.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'L', std::pair<float, float>(2.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'M', std::pair<float, float>(3.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'N', std::pair<float, float>(4.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'O', std::pair<float, float>(5.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'P', std::pair<float, float>(6.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'Q', std::pair<float, float>(7.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'R', std::pair<float, float>(8.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'S', std::pair<float, float>(9.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'T', std::pair<float, float>(10.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'U', std::pair<float, float>(11.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'V', std::pair<float, float>(12.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'W', std::pair<float, float>(13.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'X', std::pair<float, float>(14.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },
	{ 'Y', std::pair<float, float>(15.0f / LETTERS_IN_ROW, 3.0f / LETTERS_IN_COLUMN) },

	{ 'Z', std::pair<float, float>(0.0f,         4.0f / 8.0f) },
	{ 'Å', std::pair<float, float>(1.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ 'Ä', std::pair<float, float>(2.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ 'Ö', std::pair<float, float>(3.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ '.', std::pair<float, float>(4.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ '?', std::pair<float, float>(5.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ '!', std::pair<float, float>(6.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ ':', std::pair<float, float>(7.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ ',', std::pair<float, float>(8.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ ';', std::pair<float, float>(9.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ '-', std::pair<float, float>(10.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ '+', std::pair<float, float>(11.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ '/', std::pair<float, float>(12.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ '\\', std::pair<float, float>(13.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ '<', std::pair<float, float>(14.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },
	{ '>', std::pair<float, float>(15.0f / LETTERS_IN_ROW, 4.0f / LETTERS_IN_COLUMN) },

	{ '(', std::pair<float, float>(0.0f,         5.0f) },
	{ ')', std::pair<float, float>(1.0f / LETTERS_IN_ROW, 5.0f / LETTERS_IN_COLUMN) },
	{ '[', std::pair<float, float>(2.0f / LETTERS_IN_ROW, 5.0f / LETTERS_IN_COLUMN) },
	{ ']', std::pair<float, float>(3.0f / LETTERS_IN_ROW, 5.0f / LETTERS_IN_COLUMN) },
	{ '{', std::pair<float, float>(4.0f / LETTERS_IN_ROW, 5.0f / LETTERS_IN_COLUMN) },
	{ '}', std::pair<float, float>(5.0f / LETTERS_IN_ROW, 5.0f / LETTERS_IN_COLUMN) },
	{ '\'', std::pair<float, float>(6.0f / LETTERS_IN_ROW, 5.0f / LETTERS_IN_COLUMN)},
	{ '"', std::pair<float, float>(7.0f / LETTERS_IN_ROW, 5.0f / LETTERS_IN_COLUMN) },
	{ '%', std::pair<float, float>(8.0f / LETTERS_IN_ROW, 5.0f / LETTERS_IN_COLUMN) },
	{ '&', std::pair<float, float>(9.0f / LETTERS_IN_ROW, 5.0f / LETTERS_IN_COLUMN) },
	{ '*', std::pair<float, float>(10.0f / LETTERS_IN_ROW, 5.0f / LETTERS_IN_COLUMN) },
	{ ' ', std::pair<float, float>(11.0f / LETTERS_IN_ROW, 5.0f / LETTERS_IN_COLUMN) }
};