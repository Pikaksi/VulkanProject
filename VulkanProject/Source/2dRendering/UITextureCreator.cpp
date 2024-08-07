#include "UITextureCreator.hpp"

#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <unordered_set>

#include "Rendering/TextureCreator.hpp"
#include "FilePathHandler.hpp"

void createUIImageInfos(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, std::vector<ImageInfo>& uiImageInfos)
{
	int imageCount = textureToFileName.size();
	uiImageInfos.resize(imageCount);

	std::unordered_set<std::string> uiTextureFiles;

	for (auto const& dir_entry : std::filesystem::directory_iterator{ GetUITexturesDirPath() }) {
		if (dir_entry.path().extension() == ".png") {
			uiTextureFiles.insert(dir_entry.path().stem().string());
		}
	}

	for (int i = 0; i < imageCount; i++) {
		if (textureToFileName.contains(i) && uiTextureFiles.contains(textureToFileName.at(i))) {

			std::string filePath = "UITextures\\" + textureToFileName.at(i) + ".png";
			createTextureImage(vulkanCoreInfo, uiImageInfos[i], commandPool, false, filePath);
		}
		else { 
			if (textureToFileName.contains(i)) {
				std::cout << "Did not have enum " << i << ".";
			}
			else {
				std::cout << "the file corresponding to enum with the value " << i << "was not found.";
			}
			throw std::runtime_error("textureToFileName contains bad values");
		}
	}
}