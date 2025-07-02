#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "engine/VulkanRenderdata.hpp"

class VulkanLayerAndExtension
{
private:

    int gatherVulkanExtensions(VulkanRenderData& rData);

public:

    VulkanLayerAndExtension()  = default;
    ~VulkanLayerAndExtension() = default;

    

    int init(VulkanRenderData& rData);

};



