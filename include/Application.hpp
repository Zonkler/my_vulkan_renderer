#pragma once
#include "window/window.hpp"
#include "engine/VulkanRenderdata.hpp"
#include "engine/VulkanContext.hpp"
#include "engine/VulkanLayerAndExtension.hpp"
#include "engine/VulkanInstance.hpp"
#include "engine/VulkanDebug.hpp"
#include "engine/VulkanDevice.hpp"
#include "engine/VulkanSwapchain.hpp"
#include "engine/Renderer.hpp"
#include <glm/glm.hpp>
class Application{
public:
    Application(VulkanRenderData& rdata);
    ~Application() = default;


    bool running;
    
    void run();
    void processEvents();
void updateCameraPosition();
private:
    VulkanRenderData& m_rdata;
    VulkanContext m_vkContext;
    window m_window;
    VulkanLayerAndExtension m_extensions;
    VulkanInstance m_instance;
    VulkanDebug m_debug;
    VulkanDevice m_device;
    VulkanSwapchain m_swapchain;
    Renderer m_renderer;
};

