#include "engine/VulkanRenderdata.hpp"
#include <vulkan/vulkan.h>
#include "engine/VulkanInstance.hpp"
#include "tools/tools.hpp"
#include <iostream>

VulkanInstance::VulkanInstance(VulkanRenderData& rData)
{
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = rData.appname;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Skibidi";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = rData.extensions.size();
    instanceInfo.ppEnabledExtensionNames = rData.extensions.data();
    instanceInfo.enabledLayerCount = rData.layers.size();
    instanceInfo.ppEnabledLayerNames = rData.layers.data();
    
    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &vulkanInstance);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance, code: " << result << "\n";
    }

    Logger::log(0, "LOGGER::VULKAN_INSTANCE Vulkan instance created\n");
}

VulkanInstance::~VulkanInstance()
{
    Logger::log(0, "LOGGER::VULKAN_INSTANCE Vulkan instance destroyed\n");
    vkDestroyInstance(vulkanInstance,nullptr);

}

VkInstance& VulkanInstance::Get_VKinstance(){return vulkanInstance;}
