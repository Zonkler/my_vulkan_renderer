#pragma once

#include <vulkan/vulkan.h>

#include "engine/VulkanRenderdata.hpp"
#include "engine/VulkanContext.hpp"

namespace PyroCore
{
    
    class VulkanInstance
    {
    private:
        VkApplicationInfo appInfo{};
        VkInstanceCreateInfo instanceInfo{};
        VkInstance vulkanInstance;
        VkSurfaceKHR * m_vulkanSurface;
    public:
        VulkanInstance(VulkanRenderData& rData,VulkanContext& vkContext);

        VulkanInstance(const VulkanInstance&)               = delete;
        VulkanInstance(VulkanInstance&&)                    = delete;
        VulkanInstance& operator=(const VulkanInstance&)    = delete;
        VulkanInstance& operator=(VulkanInstance&&)         = delete;
                        

        ~VulkanInstance();
        
        VkInstance& Get_VKinstance();

    };

} // namespace PyroCore





