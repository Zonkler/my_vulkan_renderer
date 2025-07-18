#include <iostream>

#include <vulkan/vulkan.h>

#include "tools/tools.hpp"

#include "engine/VulkanRenderdata.hpp"
#include "engine/VulkanInstance.hpp"
#include "engine/VulkanContext.hpp"


namespace PyroCore
{
    
    VulkanInstance::VulkanInstance(VulkanRenderData& rData,VulkanContext& vkContext){

        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = rData.appname;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Skibidi";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = rData.extensions.size();
        instanceInfo.ppEnabledExtensionNames = rData.extensions.data();
        instanceInfo.enabledLayerCount = rData.layers.size();
        instanceInfo.ppEnabledLayerNames = rData.layers.data();
        
        VkResult result = vkCreateInstance(&instanceInfo, nullptr, &vulkanInstance);
        if (result != VK_SUCCESS) {
            std::cerr << "[WARNING][Logger][Vulkan Instance] Failed to create Vulkan instance, code: " << result << "\n";

        }

        vkContext.instance = &vulkanInstance;
        Logger::log(0, "[Logger][Vulkan Instance] Vulkan instance created\n");

        SDL_Vulkan_CreateSurface(rData.window, vulkanInstance, &vkContext.surface);
        m_vulkanSurface = &vkContext.surface;

    }

    VulkanInstance::~VulkanInstance(){
        
        vkDestroySurfaceKHR(vulkanInstance,*m_vulkanSurface,nullptr);
        vkDestroyInstance(vulkanInstance,nullptr);
        Logger::log(0, "[Logger][Vulkan Instance] Vulkan instance destroyed\n");

    }

    VkInstance& VulkanInstance::Get_VKinstance(){return vulkanInstance;}

} // namespace PyroCore