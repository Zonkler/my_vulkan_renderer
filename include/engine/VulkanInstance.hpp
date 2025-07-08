#pragma once

#include <vulkan/vulkan.h>

#include "engine/VulkanRenderdata.hpp"
#include "engine/VulkanContext.hpp"

class VulkanInstance
{
private:
    VkApplicationInfo appInfo{};
    VkInstanceCreateInfo instanceInfo{};
    VkInstance vulkanInstance;

public:
    VulkanInstance(VulkanRenderData& rData,VulkanContext& vkContext);
    ~VulkanInstance();
    
    VkInstance& Get_VKinstance();

};







