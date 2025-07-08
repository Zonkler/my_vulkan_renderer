#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "engine/VulkanRenderdata.hpp"

class VulkanLayerAndExtension
{
private:

    void gatherVulkanExtensions(VulkanRenderData& rData);

public:

    VulkanLayerAndExtension(VulkanRenderData& rData);
    ~VulkanLayerAndExtension() = default;

    



};



