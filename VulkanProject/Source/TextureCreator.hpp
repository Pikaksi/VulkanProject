#pragma once

#include "Application.hpp"

class TextureCreator
{
public:
	void createTextureImage(Application application);
	VkImageView createTextureImageView(Application application, VkImage image);
	void createTextureSampler();
};