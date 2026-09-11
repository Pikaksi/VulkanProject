#include "UITextureCreator.hpp"

#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <unordered_set>

#include "assertm.hpp"
#include "Rendering/TextureCreator.hpp"
#include "FilePathHandler.hpp"

void createUIImageInfos(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, std::vector<ImageInfo>& uiImageInfos)
{
    int imageCount = textureToFileName.size();
    uiImageInfos.resize(imageCount);

    std::unordered_set<std::string> uiTextureFiles;

    for (auto const& dir_entry : std::filesystem::directory_iterator{GetUITexturesDirPath()}) {
        if (dir_entry.path().extension() == ".png") {
            uiTextureFiles.insert(dir_entry.path().stem().string());
        }
    }

    for (int i = 0; i < imageCount; i++) {
        assertm(textureToFileName.contains(i), "textureToFileName table is too small compared to ui image count");
        assertm(uiTextureFiles.contains(textureToFileName.at(i)),
                "The uploaded ui images do not contain '" << textureToFileName.at(i) << ".png'");

        std::string filePath = "/UITextures/" + textureToFileName.at(i) + ".png";
        createTextureImage(vulkanCoreInfo, uiImageInfos[i], commandPool, false, filePath);
    }
}
