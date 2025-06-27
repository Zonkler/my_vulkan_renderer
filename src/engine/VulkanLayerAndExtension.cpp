#include "engine/VulkanLayerAndExtension.hpp"
#include <iostream>
#include "tools/tools.hpp"
#include "engine/VulkanRenderdata.hpp"
VulkanLayerAndExtension::VulkanLayerAndExtension(VulkanRenderData& rData){
    gatherVulkanExtensions(rData);

}


int VulkanLayerAndExtension::gatherVulkanExtensions(VulkanRenderData& rData){
    
    uint32_t extensionCount = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(rData.window, &extensionCount, nullptr)) {
        std::cerr << "Failed to get extension count\n";
        return -1;
    }

    rData.extensions.resize(extensionCount);

    if (!SDL_Vulkan_GetInstanceExtensions(rData.window, &extensionCount, rData.extensions.data())) {
        std::cerr << "Failed to get extension names\n";
        return -1;
    }

    rData.extensions.push_back("VK_EXT_debug_utils"); // Add debug extension
    rData.layers.push_back("VK_LAYER_KHRONOS_validation"); // Add validation layer

    Logger::log(0, "LOGGER::VULKANLAYER:: SDL and Vulkan extensions retrieved\n");

    return 0;
}

