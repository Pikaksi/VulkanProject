#include "UIText.hpp"

#include <iostream>

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
		glm::vec2 texLocation = charToTexLocation.at(text[i]);
		UIQuad uIQuad(
			location + letterSize * glm::vec2(xCounter / 2.0f, yCounter),
			letterSize,
			texLocation,
			texLocation + LETTER_TEX_SIZE,
			0,
			{0.0f, 0.0f, 0.0f, 0.0f },
			letterCenteringMode);
		textQuads.push_back(uIQuad);

		xCounter++;
	}
}

void UIText::addMeshData(VkExtent2D extent, std::vector<Vertex2D>& vertices)
{
	for (const UIQuad& charQuad : textQuads) {
		charQuad.addMeshData(extent, vertices);
	}
}
