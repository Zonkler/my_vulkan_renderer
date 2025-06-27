#pragma once
#include "engine/VulkanRenderdata.hpp"
#include <vulkan/vulkan.h>

class VulkanInstance
{
private:
    VkApplicationInfo appInfo{};
    VkInstanceCreateInfo instanceInfo{};
    VkInstance vulkanInstance;

   

public:
    VulkanInstance(VulkanRenderData& rData);
    ~VulkanInstance();
    VkInstance& Get_VKinstance();
};







