#pragma once

#include <vulkan/vulkan.h>

struct RenderTarget {

    void Destroy(VkDevice device) noexcept {

        if (Sampler) {
            vkDestroySampler(device, Sampler, nullptr);
            Sampler = VK_NULL_HANDLE;
        }

        if (View) {
            vkDestroyImageView(device, View, nullptr);
            View = VK_NULL_HANDLE;
        }

        if (Image) {
            vkDestroyImage(device, Image, nullptr);
            Image = VK_NULL_HANDLE;
        }

        if (Memory) {
            vkFreeMemory(device, Memory, nullptr);
            Memory = VK_NULL_HANDLE;
        }

    }

    VkImage Image = VK_NULL_HANDLE;
    VkImageView View = VK_NULL_HANDLE;
    VkSampler Sampler = VK_NULL_HANDLE;
    VkDeviceMemory Memory = VK_NULL_HANDLE;

    VkFormat Format = VK_FORMAT_UNDEFINED;

    uint32_t Width = 0;
    uint32_t Height = 0;

};
