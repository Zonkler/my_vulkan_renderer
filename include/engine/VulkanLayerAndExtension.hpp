#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "engine/VulkanRenderdata.hpp"
class VulkanLayerAndExtension
{
private:
    
public:
    VulkanLayerAndExtension(VulkanRenderData& rData);
    int gatherVulkanExtensions(VulkanRenderData& rData);
    
};



