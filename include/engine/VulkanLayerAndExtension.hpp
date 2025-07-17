#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "engine/VulkanRenderdata.hpp"

/*
    NOTE:  
    Non-copyable, non-movable: Retrieves all the extensions and layers needed
    for Vulkan using SDL and also adds additional debug validation layers.
*/

namespace PyroCore
{
    class VulkanLayerAndExtension
    {
    private:

        void gatherVulkanExtensions(VulkanRenderData& rData);

    public:

        VulkanLayerAndExtension(VulkanRenderData& rData);

        VulkanLayerAndExtension(const VulkanLayerAndExtension&)            = delete;
        VulkanLayerAndExtension(VulkanLayerAndExtension&&)                 = delete;
        VulkanLayerAndExtension& operator=(const VulkanLayerAndExtension&) = delete;
        VulkanLayerAndExtension& operator=(VulkanLayerAndExtension&&)      = delete;

        ~VulkanLayerAndExtension() = default;

    };

} //namespace PyroCore

