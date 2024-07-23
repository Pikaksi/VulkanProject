#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "UIQuad.hpp"

const float LETTERS_IN_TEX_ROW = 16.0f;
const float LETTERS_IN_TEX_COLUMN = 8.0f;

const float LETTER_TEX_WIDTH = 1.0f / LETTERS_IN_TEX_ROW;
const float LETTER_TEX_HEIGHT = 1.0f / LETTERS_IN_TEX_COLUMN;
const glm::vec2 LETTER_TEX_SIZE = { LETTER_TEX_WIDTH, LETTER_TEX_HEIGHT };

struct UIText
{
public:
	glm::vec2 location;
	glm::vec2 letterSize;
	UICenteringMode letterCenteringMode;

	void setText(std::string text);
	void addMeshData(VkExtent2D extent, std::vector<Vertex2D>& vertices);

	UIText() {}
	UIText(glm::vec2 location, float letterHeight, std::string text, UICenteringMode letterCenteringMode) :
		location(location), letterSize({ letterHeight / 2, letterHeight }), letterCenteringMode(letterCenteringMode)
	{
		setText(text);
	}

private:
	std::vector<UIQuad> textQuads;
};

const std::unordered_map<char, glm::vec2> charToTexLocation = std::unordered_map<char, glm::vec2>{
	{ '0', {0.0f,                       0.0f} },
	{ '1', {1.0f / LETTERS_IN_TEX_ROW,  0.0f} },
	{ '2', {2.0f / LETTERS_IN_TEX_ROW,  0.0f} },
	{ '3', {3.0f / LETTERS_IN_TEX_ROW,  0.0f} },
	{ '4', {4.0f / LETTERS_IN_TEX_ROW,  0.0f} },
	{ '5', {5.0f / LETTERS_IN_TEX_ROW,  0.0f} },
	{ '6', {6.0f / LETTERS_IN_TEX_ROW,  0.0f} },
	{ '7', {7.0f / LETTERS_IN_TEX_ROW,  0.0f} },
	{ '8', {8.0f / LETTERS_IN_TEX_ROW,  0.0f} },
	{ '9', {9.0f / LETTERS_IN_TEX_ROW,  0.0f} },
	{ 'a', {10.0f / LETTERS_IN_TEX_ROW, 0.0f} },
	{ 'b', {11.0f / LETTERS_IN_TEX_ROW, 0.0f} },
	{ 'c', {12.0f / LETTERS_IN_TEX_ROW, 0.0f} },
	{ 'd', {13.0f / LETTERS_IN_TEX_ROW, 0.0f} },
	{ 'e', {14.0f / LETTERS_IN_TEX_ROW, 0.0f} },
	{ 'f', {15.0f / LETTERS_IN_TEX_ROW, 0.0f} },

	{ 'g', {0.0f,					    1.0f / 8.0f } },
	{ 'h', {1.0f / LETTERS_IN_TEX_ROW,  1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'i', {2.0f / LETTERS_IN_TEX_ROW,  1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'j', {3.0f / LETTERS_IN_TEX_ROW,  1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'k', {4.0f / LETTERS_IN_TEX_ROW,  1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'l', {5.0f / LETTERS_IN_TEX_ROW,  1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'm', {6.0f / LETTERS_IN_TEX_ROW,  1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'n', {7.0f / LETTERS_IN_TEX_ROW,  1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'o', {8.0f / LETTERS_IN_TEX_ROW,  1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'p', {9.0f / LETTERS_IN_TEX_ROW,  1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'q', {10.0f / LETTERS_IN_TEX_ROW, 1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'r', {11.0f / LETTERS_IN_TEX_ROW, 1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 's', {12.0f / LETTERS_IN_TEX_ROW, 1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 't', {13.0f / LETTERS_IN_TEX_ROW, 1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'u', {14.0f / LETTERS_IN_TEX_ROW, 1.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'v', {15.0f / LETTERS_IN_TEX_ROW, 1.0f / LETTERS_IN_TEX_COLUMN} },

	{ 'w', {0.0f,                       2.0f / 8.0f } },
	{ 'x', {1.0f / LETTERS_IN_TEX_ROW,  2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'y', {2.0f / LETTERS_IN_TEX_ROW,  2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'z', {3.0f / LETTERS_IN_TEX_ROW,  2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'å', {4.0f / LETTERS_IN_TEX_ROW,  2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'ä', {5.0f / LETTERS_IN_TEX_ROW,  2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'ö', {6.0f / LETTERS_IN_TEX_ROW,  2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'A', {7.0f / LETTERS_IN_TEX_ROW,  2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'B', {8.0f / LETTERS_IN_TEX_ROW,  2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'C', {9.0f / LETTERS_IN_TEX_ROW,  2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'D', {10.0f / LETTERS_IN_TEX_ROW, 2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'E', {11.0f / LETTERS_IN_TEX_ROW, 2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'F', {12.0f / LETTERS_IN_TEX_ROW, 2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'G', {13.0f / LETTERS_IN_TEX_ROW, 2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'H', {14.0f / LETTERS_IN_TEX_ROW, 2.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'I', {15.0f / LETTERS_IN_TEX_ROW, 2.0f / LETTERS_IN_TEX_COLUMN} },

	{ 'J', {0.0f,					    3.0f / 8.0f } },
	{ 'K', {1.0f / LETTERS_IN_TEX_ROW,  3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'L', {2.0f / LETTERS_IN_TEX_ROW,  3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'M', {3.0f / LETTERS_IN_TEX_ROW,  3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'N', {4.0f / LETTERS_IN_TEX_ROW,  3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'O', {5.0f / LETTERS_IN_TEX_ROW,  3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'P', {6.0f / LETTERS_IN_TEX_ROW,  3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'Q', {7.0f / LETTERS_IN_TEX_ROW,  3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'R', {8.0f / LETTERS_IN_TEX_ROW,  3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'S', {9.0f / LETTERS_IN_TEX_ROW,  3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'T', {10.0f / LETTERS_IN_TEX_ROW, 3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'U', {11.0f / LETTERS_IN_TEX_ROW, 3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'V', {12.0f / LETTERS_IN_TEX_ROW, 3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'W', {13.0f / LETTERS_IN_TEX_ROW, 3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'X', {14.0f / LETTERS_IN_TEX_ROW, 3.0f / LETTERS_IN_TEX_COLUMN} },
	{ 'Y', {15.0f / LETTERS_IN_TEX_ROW, 3.0f / LETTERS_IN_TEX_COLUMN} },

	{ 'Z', {0.0f,					    4.0f / 8.0f } },
	{ 'Å', {1.0f / LETTERS_IN_TEX_ROW,  4.0f / LETTERS_IN_TEX_COLUMN } },
	{ 'Ä', {2.0f / LETTERS_IN_TEX_ROW,  4.0f / LETTERS_IN_TEX_COLUMN } },
	{ 'Ö', {3.0f / LETTERS_IN_TEX_ROW,  4.0f / LETTERS_IN_TEX_COLUMN } },
	{ '.', {4.0f / LETTERS_IN_TEX_ROW,  4.0f / LETTERS_IN_TEX_COLUMN } },
	{ '?', {5.0f / LETTERS_IN_TEX_ROW,  4.0f / LETTERS_IN_TEX_COLUMN } },
	{ '!', {6.0f / LETTERS_IN_TEX_ROW,  4.0f / LETTERS_IN_TEX_COLUMN } },
	{ ':', {7.0f / LETTERS_IN_TEX_ROW,  4.0f / LETTERS_IN_TEX_COLUMN } },
	{ ',', {8.0f / LETTERS_IN_TEX_ROW,  4.0f / LETTERS_IN_TEX_COLUMN } },
	{ ';', {9.0f / LETTERS_IN_TEX_ROW,  4.0f / LETTERS_IN_TEX_COLUMN } },
	{ '-', {10.0f / LETTERS_IN_TEX_ROW, 4.0f / LETTERS_IN_TEX_COLUMN } },
	{ '+', {11.0f / LETTERS_IN_TEX_ROW, 4.0f / LETTERS_IN_TEX_COLUMN } },
	{ '/', {12.0f / LETTERS_IN_TEX_ROW, 4.0f / LETTERS_IN_TEX_COLUMN } },
	{ '\\',{13.0f / LETTERS_IN_TEX_ROW, 4.0f / LETTERS_IN_TEX_COLUMN } },
	{ '<', {14.0f / LETTERS_IN_TEX_ROW, 4.0f / LETTERS_IN_TEX_COLUMN } },
	{ '>', {15.0f / LETTERS_IN_TEX_ROW, 4.0f / LETTERS_IN_TEX_COLUMN } },

	{ '(', {0.0f,					    5.0f / 8.0f } },
	{ ')', {1.0f / LETTERS_IN_TEX_ROW,  5.0f / LETTERS_IN_TEX_COLUMN } },
	{ '[', {2.0f / LETTERS_IN_TEX_ROW,  5.0f / LETTERS_IN_TEX_COLUMN } },
	{ ']', {3.0f / LETTERS_IN_TEX_ROW,  5.0f / LETTERS_IN_TEX_COLUMN } },
	{ '{', {4.0f / LETTERS_IN_TEX_ROW,  5.0f / LETTERS_IN_TEX_COLUMN } },
	{ '}', {5.0f / LETTERS_IN_TEX_ROW,  5.0f / LETTERS_IN_TEX_COLUMN } },
	{ '\'',{6.0f / LETTERS_IN_TEX_ROW,  5.0f / LETTERS_IN_TEX_COLUMN } },
	{ '"', {7.0f / LETTERS_IN_TEX_ROW,  5.0f / LETTERS_IN_TEX_COLUMN } },
	{ '%', {8.0f / LETTERS_IN_TEX_ROW,  5.0f / LETTERS_IN_TEX_COLUMN } },
	{ '&', {9.0f / LETTERS_IN_TEX_ROW,  5.0f / LETTERS_IN_TEX_COLUMN } },
	{ '*', {10.0f / LETTERS_IN_TEX_ROW, 5.0f / LETTERS_IN_TEX_COLUMN } },
	{ ' ', {11.0f / LETTERS_IN_TEX_ROW, 5.0f / LETTERS_IN_TEX_COLUMN } }
};