#include "UIText.hpp"

#include <iostream>

#include "UIHelperFunctions.hpp"

void UIText::addMeshData(VkExtent2D extent, std::vector<Vertex2D>& vertices)
{
	int xCounter = 0;
	int yCounter = 0;
	glm::vec2 scalar = getScalarFromExtent(extent);

	for (int i = 0; i < text.size(); i++) {
		if (text[i] == '\n') {
			xCounter = 0;
			yCounter++;
			continue;
		}
		if (text[i] == ' ' || !charToTexLocation.contains(text[i])) {
			xCounter++;
			continue;
		}
		glm::vec2 texLocation = charToTexLocation.at(text[i]);

		UIQuad uiQuad(
			location + letterSize * glm::vec2(xCounter, yCounter) * scalar,
			letterSize * scalar,
			texLocation,
			texLocation + LETTER_TEX_SIZE,
			UITexLayer::text,
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			letterCenteringMode,
			false); // Scaling is already accounted for. This saves performance.
		uiQuad.addMeshData(extent, vertices);

		xCounter++;
	}
}
