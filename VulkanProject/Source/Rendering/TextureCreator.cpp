#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>
#include <iostream>

#include "TextureCreator.hpp"
#include "VulkanRendering/ImageCreator.hpp"
#include "VulkanRendering/Buffers.hpp"
#include "FilePathHandler.hpp"


void createTextureImage(VulkanCoreInfo* vulkanCoreInfo, ImageInfo* imageInfo, VkCommandPool commandPool, bool generateMipLevels, std::string fileName) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(("Textures/" + fileName).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    
    uint32_t mipLevels = 1;
    if (generateMipLevels) {
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
    }

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(vulkanCoreInfo, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vulkanCoreInfo->device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(vulkanCoreInfo->device, stagingBufferMemory);

    stbi_image_free(pixels);

    createImageInfo(
        vulkanCoreInfo,
        imageInfo,
        texWidth,
        texHeight,
        mipLevels,
        VK_SAMPLE_COUNT_1_BIT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        1,
        VK_IMAGE_VIEW_TYPE_2D);

    transitionImageLayout(vulkanCoreInfo, commandPool, imageInfo->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, 1);
    copyBufferToImage(vulkanCoreInfo, commandPool, stagingBuffer, imageInfo->image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1);
    //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

    vkDestroyBuffer(vulkanCoreInfo->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, stagingBufferMemory, nullptr);

    generateMipmaps(vulkanCoreInfo, commandPool, imageInfo->image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels, 1);
}

VkSampler createBlockTextureSampler(VulkanCoreInfo* vulkanCoreInfo) {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(vulkanCoreInfo->physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(1);
    samplerInfo.mipLodBias = 0.0f;

    VkSampler textureSampler;
    if (vkCreateSampler(vulkanCoreInfo->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
    return textureSampler;
}

VkSampler createTextTextureSampler(VulkanCoreInfo* vulkanCoreInfo)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(vulkanCoreInfo->physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(1);
    samplerInfo.mipLodBias = 0.0f;

    VkSampler textureSampler;
    if (vkCreateSampler(vulkanCoreInfo->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
    return textureSampler;
}

void createBlockTextureArray(VulkanCoreInfo* vulkanCoreInfo, ImageInfo* imageInfo, VkCommandPool commandPool, bool generateMipLevels) {

    std::vector<stbi_uc*> imagePixels;
    std::vector<int> texWidths, texHeights;
    int texChannels;

    for (auto const& dir_entry : std::filesystem::directory_iterator{ GetBlockTexturesDirPath() }) {
        if (dir_entry.path().extension() == ".png") {

            std::cout << dir_entry.path().filename().string() << " a\n";

            texWidths.push_back(0);
            texHeights.push_back(0);
            imagePixels.push_back(stbi_load(("Textures/BlockTextures/" + dir_entry.path().filename().string()).c_str(), &texWidths.back(), &texHeights.back(), &texChannels, STBI_rgb_alpha));

            if (!imagePixels.back()) {
                throw std::runtime_error("failed to load texture image!");
            }
        }
    }
    uint32_t imageCount = imagePixels.size();


    for (int i = 0; i < texWidths.size() - 1; i++) {
        if (texWidths[i] != texWidths[i + 1] || texHeights[i] != texHeights[i + 1]) {
            throw std::runtime_error("Block texture images are different sizes!");
        }
    }
    if (texWidths[0] != BLOCK_TEXTURE_PIXEL_COUNT || texHeights[0] != BLOCK_TEXTURE_PIXEL_COUNT) {
        throw std::runtime_error("BLOCK_TEXTURE_PIXEL_COUNT constant has wrong value!");
    }

    int texWidth = texWidths[0];
    int texHeight = texHeights[0];

    uint32_t mipLevels = 1;
    if (generateMipLevels) {
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
    }

    createImageInfo(
        vulkanCoreInfo,
        imageInfo,
        texWidths[0],
        texHeights[0],
        mipLevels,
        VK_SAMPLE_COUNT_1_BIT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        imageCount,
        VK_IMAGE_VIEW_TYPE_2D_ARRAY);

    VkDeviceSize imageSize = texWidth * texHeight * 4;
    VkDeviceSize imageArraySize = imageSize * imageCount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(vulkanCoreInfo, imageArraySize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vulkanCoreInfo->device, stagingBufferMemory, 0, imageArraySize, 0, &data);

    for (int i = 0; i < imagePixels.size(); i++) {
        memcpy(static_cast<stbi_uc*>(data) + i * imageSize, imagePixels[i], static_cast<size_t>(imageSize));
        stbi_image_free(imagePixels[i]);
    }
    vkUnmapMemory(vulkanCoreInfo->device, stagingBufferMemory);


    transitionImageLayout(vulkanCoreInfo, commandPool, imageInfo->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, imageCount);
    copyBufferToImage(vulkanCoreInfo, commandPool, stagingBuffer, imageInfo->image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), imageCount);
    //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

    vkDestroyBuffer(vulkanCoreInfo->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, stagingBufferMemory, nullptr);

    generateMipmaps(vulkanCoreInfo, commandPool, imageInfo->image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels, imageCount);
}