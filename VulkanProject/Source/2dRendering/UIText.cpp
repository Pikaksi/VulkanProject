#include "UIText.hpp"

#include <iostream>

#include "UIHelperFunctions.hpp"

void createUIText(
	UIManager& uiManager,
	glm::vec2 location,
	float letterHeight,
	std::string text,
	UICenteringMode letterCenteringMode,
	UICenteringMode textBoxCenteringMode)
{
	int xCounter = 0;
	int yCounter = 0;
	glm::vec2 letterSize = {letterHeight, letterHeight / 2};

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

		createUIQuad(
			location + letterSize * glm::vec2(xCounter, yCounter) * uiManager.getScalar(),
			letterSize * uiManager.getScalar(),
			texLocation,
			texLocation + LETTER_TEX_SIZE,
			UITexLayer::text,
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			letterCenteringMode,
			false); // Scaling is already accounted for. This saves performance.

		xCounter++;
	}
}
