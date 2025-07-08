#include "Application.hpp"
#include "tools/exception.hpp"


Application::Application(VulkanRenderData& rdata) :
        m_rdata(rdata),
        m_window(m_rdata),
        m_extensions(m_rdata),
        m_instance(m_rdata,m_vkContext),
        m_debug(*m_vkContext.instance),
        m_device(m_vkContext,{VK_KHR_SWAPCHAIN_EXTENSION_NAME}),
        m_swapchain(m_rdata,m_vkContext,m_device),
        m_renderer(m_rdata,m_vkContext,m_swapchain.swapChain,m_swapchain)
{


};

void Application::run(){
    running = true;
    while (running)
    {
        processEvents();
        m_renderer.renderFrame();
    }
    vkDeviceWaitIdle(*m_vkContext.device);
    
}

void Application::processEvents(){
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        
        if (event.type == SDL_QUIT)
        {
         running = false;
        }
    }
}