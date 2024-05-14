#include "TextureCreator.hpp"
#include "VulkanRendering/ImageCreator.hpp"
#include "VulkanRendering/Buffers.hpp"

void createTextureImage(VulkanCoreInfo* vulkanCoreInfo, ImageInfo* imageInfo, VkCommandPool commandPool, bool generateMipLevels) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("Textures/GrassTest.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
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
        VK_IMAGE_ASPECT_COLOR_BIT);

    transitionImageLayout(vulkanCoreInfo, commandPool, imageInfo->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
    copyBufferToImage(vulkanCoreInfo, commandPool, stagingBuffer, imageInfo->image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

    vkDestroyBuffer(vulkanCoreInfo->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, stagingBufferMemory, nullptr);

    generateMipmaps(vulkanCoreInfo, commandPool, imageInfo->image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
}

