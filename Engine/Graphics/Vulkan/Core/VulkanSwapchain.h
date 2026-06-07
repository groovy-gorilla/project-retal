#pragma once

struct Settings;

class VulkanSwapchain {
public:
    void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkExtent2D extent, uint32_t graphicsQueueFamily, uint32_t presentQueueFamily, bool vsync);
    void Destroy(VkDevice device);

    [[nodiscard]] VkSwapchainKHR Get() const { return m_swapchain; }
    [[nodiscard]] const std::vector<VkImage>& GetImages() const { return m_images; }
    [[nodiscard]] const std::vector<VkImageView>& GetImageViews() const { return m_imageViews; }
    [[nodiscard]] VkFormat GetImageFormat() const { return m_imageFormat; }
    [[nodiscard]] VkExtent2D GetExtent() const { return m_extent; }

    void CreateFramebuffers(VkDevice device, VkRenderPass renderPass, VkExtent2D windowExtent);
    [[nodiscard]] VkFramebuffer GetFramebuffer(uint32_t index) const { return m_framebuffers[index]; }

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    VkFormat m_imageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D m_extent = {};

    std::vector<VkFramebuffer> m_framebuffers;


    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes, bool vsync);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,  VkExtent2D extent);
    void CreateImageViews(VkDevice device);
    void DestroyImageViews(VkDevice device);

};
