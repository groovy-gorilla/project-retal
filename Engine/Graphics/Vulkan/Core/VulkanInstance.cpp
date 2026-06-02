#include "pch.h"
#include "VulkanInstance.h"
#include "VulkanValidation.h"
#include "Core/ApplicationDesc.h"
#include "VulkanDebug.h"
#include "Debug/ErrorDialog.h"

void VulkanInstance::Create() {

    #ifndef NDEBUG
        if (!VulkanValidation::CheckSupport()) {
            throw std::runtime_error("Validation layers are not available!");
        }
    #endif

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "TFX";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "Indigo";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;

    VkValidationFeatureEnableEXT enables[] = {
        VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
        //VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
        //VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT
    };

    VkValidationFeaturesEXT validationFeatures{};
    validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
    validationFeatures.pNext = nullptr;
    validationFeatures.enabledValidationFeatureCount = static_cast<uint32_t>(std::size(enables));
    validationFeatures.pEnabledValidationFeatures = enables;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    #ifndef NDEBUG
        createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanValidation::Layers.size());
        createInfo.ppEnabledLayerNames = VulkanValidation::Layers.data();
        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        debugCreateInfo.pNext = &validationFeatures;
        createInfo.pNext = &debugCreateInfo;
    #else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    #endif

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_instance));

    std::cout << "[Vulkan] Instance created" << std::endl;

}

void VulkanInstance::Destroy() {

    if (m_instance) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Instance destroyed" << std::endl;
    }

}

VkInstance VulkanInstance::Get() const {
    return m_instance;
}

std::vector<const char*> VulkanInstance::GetRequiredExtensions() {

    uint32_t count;

    const char* const* ext = SDL_Vulkan_GetInstanceExtensions(&count);
    if (!ext) {
        throw std::runtime_error(SDL_GetError());
    }

    std::vector<const char*> extensions(ext, ext + count);

    if (true) { // validation enable
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;

}

void VulkanInstance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {

    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = VulkanDebug::DebugCallback;

}

