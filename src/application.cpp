#include "Application.hpp"
#include "tools/exception.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/VulkanShaderStorageBuffer.hpp"
#include "engine/VulkanRenderdata.hpp"
Application::Application(VulkanRenderData& rdata) :
        m_rdata(rdata),
        m_window(m_rdata),
        m_extensions(m_rdata),
        m_instance(m_rdata,m_vkContext),
        m_debug(*m_vkContext.instance),
        m_device(m_vkContext,{VK_KHR_SWAPCHAIN_EXTENSION_NAME}),
        m_swapchain(std::make_shared<VulkanSwapchain>(m_rdata,m_vkContext,m_device)),
        m_renderer(m_rdata,m_vkContext,m_swapchain)
{


};

void Application::run(){
    VkShaderStorageBufferData m_cameraSSBO;

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
    switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                std::cout<<"Changed window size to "<<  event.window.data1<<" and "<<  event.window.data2<<'\n';
                m_rdata.rdWidth = event.window.data1;
                m_rdata.rdHeight = event.window.data2;
                vkDeviceWaitIdle(*m_vkContext.device);
                m_swapchain.reset();
                m_swapchain = std::make_shared<VulkanSwapchain>(m_rdata,m_vkContext,m_device);
                m_renderer.recreateSwapchain(m_swapchain);
            }

    }
}
}
