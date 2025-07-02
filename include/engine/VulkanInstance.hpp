#pragma once

#include <vulkan/vulkan.h>

#include "engine/VulkanRenderdata.hpp"

class VulkanInstance
{
private:
    VkApplicationInfo appInfo{};
    VkInstanceCreateInfo instanceInfo{};
    VkInstance vulkanInstance;

public:
    VulkanInstance() = default;
    ~VulkanInstance()= default;
    
    int init(VulkanRenderData& rData);
    void destroy();
    
    VkInstance& Get_VKinstance();

};







