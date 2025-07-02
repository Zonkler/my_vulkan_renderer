#include <iostream>

#include "tools/tools.hpp"

#include "engine/VulkanLayerAndExtension.hpp"
#include "engine/VulkanRenderdata.hpp"

int VulkanLayerAndExtension::init(VulkanRenderData& rData){
    return gatherVulkanExtensions(rData);
}

int VulkanLayerAndExtension::gatherVulkanExtensions(VulkanRenderData& rData){
    
    uint32_t extensionCount = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(rData.window, &extensionCount, nullptr)) {
        std::cerr << "[WARNING][Logger][Vulkan layer & extension] Failed to get extension count\n";
        return 0;
    }

    rData.extensions.resize(extensionCount);

    if (!SDL_Vulkan_GetInstanceExtensions(rData.window, &extensionCount, rData.extensions.data())) {
        std::cerr << "[WARNING][Logger][Vulkan layer & extension] Failed to get extension names\n";
        return 0;
    }

    rData.extensions.push_back("VK_EXT_debug_utils"); // Add debug extension
    rData.layers.push_back("VK_LAYER_KHRONOS_validation"); // Add validation layer

    Logger::log(0, "[Logger][Vulkan layer & extension] SDL and Vulkan extensions retrieved\n");

    
    Logger::log(0, "[Logger][Vulkan layer & extension] Vulkan extensions: \n");
    for (auto extension_name : rData.extensions)
    {
        std::cout<<'\t'<<extension_name<<'\n';
    }

    return 1;

}

