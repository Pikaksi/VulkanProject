#include "UIText.hpp"

#include <iostream>

#include "UIHelperFunctions.hpp"

void createUIText(
	UIManager& uiManager,
	glm::vec2 location,
	float letterHeight,
	UICenteringMode letterCenteringMode,
	UICenteringMode textBoxCenteringMode,
	std::string text)
{
	int xCounter = 0;
	int yCounter = 0;

	glm::vec2 letterSize = glm::vec2(letterHeight / 2.0f, letterHeight);
	centerLocation(location, letterSize, letterCenteringMode);
	letterSize *= uiManager.getScalar();

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
			uiManager,
			location + glm::vec2(xCounter, yCounter) * letterSize,
			letterSize,
			texLocation,
			texLocation + LETTER_TEX_SIZE,
			UITexLayer::text,
			{ 0.0f, 0.0f, 0.0f, 1.0f });

		xCounter++;
	}
}
