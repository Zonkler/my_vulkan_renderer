#include "engine/VulkanDebug.hpp"
#include "tools/tools.hpp"
#include <iostream>

VulkanDebug::VulkanDebug(VkInstance instance) : instance(instance) {
    setupDebugMessenger();
    Logger::log(0,"LOGGER::DEBUG:: Debug created\n");

}

VulkanDebug::~VulkanDebug() {
    if (debugMessenger != VK_NULL_HANDLE) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func) {
            func(instance, debugMessenger, nullptr);
        }
    }

    Logger::log(0,"LOGGER::DEBUG:: Debug destroyed\n");

}

void VulkanDebug::setupDebugMessenger() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func && func(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        std::cerr << "Failed to set up debug messenger!\n";
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebug::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* /*userData*/) {

    std::cerr <<"[Validation Layer] " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
