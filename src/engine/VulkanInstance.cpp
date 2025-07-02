#include <iostream>

#include <vulkan/vulkan.h>

#include "tools/tools.hpp"

#include "engine/VulkanRenderdata.hpp"
#include "engine/VulkanInstance.hpp"

int VulkanInstance::init(VulkanRenderData& rData){

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
        std::cerr << "[WARNING][Logger][Vulkan Instance] Failed to create Vulkan instance, code: " << result << "\n";
        return 0;
    }

    Logger::log(0, "[Logger][Vulkan Instance] Vulkan instance created\n");
    return 1;
}

void VulkanInstance::destroy(){
    
    vkDestroyInstance(vulkanInstance,nullptr);
    Logger::log(0, "[Logger][Vulkan Instance] Vulkan instance destroyed\n");

}

VkInstance& VulkanInstance::Get_VKinstance(){return vulkanInstance;}
