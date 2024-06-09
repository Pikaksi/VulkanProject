#include "UIText.hpp"

void UIText::setText(std::string text)
{
	textQuads.clear();
	textQuads = std::vector<UIQuad>();

	int xCounter = 0;
	int yCounter = 0;

	for (int i = 0; i < text.size(); i++) {
		if (text[i] == '\n') {
			xCounter = 0;
			yCounter++;
			continue;
		}
		if (!charToTexLocation.contains(text[i])) {
			continue;
		}
		std::pair<float, float> texLocation = charToTexLocation.at(text[i]);
		UIQuad uIQuad(
			// dont know why the number here is 4 instead of 2
			x + ((letterHeight / 4 ) * xCounter), y + (letterHeight * yCounter),
			letterHeight / 2, letterHeight,
			texLocation.first, texLocation.second,
			LETTER_TEX_WIDTH, LETTER_TEX_HEIGHT);
		textQuads.push_back(uIQuad);

		xCounter++;
	}
}

void UIText::addMeshData(int screenWidth, int screenHeight, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
	for (const UIQuad& charQuad : textQuads) {
		charQuad.addMeshData(screenWidth, screenHeight, vertices, indices);
	}
}
