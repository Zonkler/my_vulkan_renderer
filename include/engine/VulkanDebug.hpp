#pragma once
#include <vulkan/vulkan.h>

class VulkanDebug {
public:

    VulkanDebug(VkInstance& instance);
    ~VulkanDebug();

    int init(VkInstance& instance);
    void destroy();

private:
    VkInstance * m_instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    void setupDebugMessenger();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
};
