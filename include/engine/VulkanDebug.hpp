#pragma once
#include <vulkan/vulkan.h>

namespace PyroCore{

    class VulkanDebug {
    public:

        VulkanDebug(VkInstance& instance);

        VulkanDebug(const VulkanDebug&)             = delete;    
        VulkanDebug(VulkanDebug&&)                  = delete;
        VulkanDebug operator=(const VulkanDebug&)   = delete;
        VulkanDebug operator=(VulkanDebug&&)        = delete;
        
        ~VulkanDebug();

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

} //namespace PyroCore