#pragma once
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"

std::vector<char> ReadFile(
    const std::string& filename);

uint32_t FindMemoryType(
    VkPhysicalDevice physicalDevice,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties);

VkFormat FindDepthFormat(
    VkPhysicalDevice physicalDevice);

void CreateImageResource(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageUsageFlags usage,
    VkSampleCountFlagBits samples,
    VkImage& image,
    VkDeviceMemory& memory);

void CreateImageViewResource(
    VkDevice device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspect,
    VkImageView& imageView);

void CreateSamplerResource(
    VkDevice& device,
    VkSampler& sampler);

VkShaderModule CreateShaderModule(
    VkDevice device,
    const std::vector<char>& code);

VkCommandBuffer BeginSingleTimeCommands(
    VkDevice device,
    VkCommandPool commandPool);

void EndSingleTimeCommands(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    VkCommandBuffer commandBuffer);

void TransitionImageLayoutImmediate(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    VkImage image,
    VkImageAspectFlags aspectMask,
    VkImageLayout oldLayout,
    VkImageLayout newLayout
);

void TransitionImageLayout(
    VkCommandBuffer commandBuffer,
    RenderTarget& target,
    VkImageLayout newLayout);

void TransitionImageLayout(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageAspectFlags aspectMask,
    VkImageLayout oldLayout,
    VkImageLayout newLayout);


void CopyBufferToImage(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    VkBuffer buffer,
    VkImage image,
    uint32_t width,
    uint32_t height);

const char* VkFormatToString(
    VkFormat format);



