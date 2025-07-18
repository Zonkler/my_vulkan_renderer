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
#include <memory>
#include <glm/glm.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
class Application{
public:
    Application(VulkanRenderData& rdata);
    ~Application() = default;


    bool running;
    bool resized = false;
    void run();
    void processEvents();

private:
    VulkanRenderData& m_rdata;
    VulkanContext m_vkContext;
    PyroCore::window m_window;
    PyroCore::VulkanLayerAndExtension m_extensions;
    PyroCore::VulkanInstance m_instance;
    PyroCore::VulkanDebug m_debug;
    PyroCore::VulkanDevice m_device;
    std::shared_ptr<PyroCore::VulkanSwapchain> m_swapchain;
    PyroCore::Renderer m_renderer;
};

