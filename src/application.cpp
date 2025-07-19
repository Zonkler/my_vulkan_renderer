#include "Application.hpp"
#include "tools/exception.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/VulkanShaderStorageBuffer.hpp"
#include "engine/VulkanRenderdata.hpp"
#include <chrono>
Application::Application(VulkanRenderData &rdata) : m_rdata(rdata),
                                                    m_window(m_rdata),
                                                    m_extensions(m_rdata),
                                                    m_instance(m_rdata, m_vkContext),
                                                    m_debug(*m_vkContext.instance),
                                                    m_device(m_vkContext, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}),
                                                    m_swapchain(std::make_shared<PyroCore::VulkanSwapchain>(m_rdata, m_vkContext, m_device)),
                                                    m_renderer(m_rdata, m_vkContext, m_swapchain) {};

void Application::run()
{
    running = true;

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (running)
    {
        processEvents();

        //delta time
        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        const Uint8* keystate = SDL_GetKeyboardState(nullptr);

        m_rdata.rdMoveForward = 0;
        if (keystate[SDL_SCANCODE_W]) m_rdata.rdMoveForward += 1;
        if (keystate[SDL_SCANCODE_S]) m_rdata.rdMoveForward -= 1;

        m_rdata.rdMoveRight = 0;
        if (keystate[SDL_SCANCODE_D]) m_rdata.rdMoveRight += 1;
        if (keystate[SDL_SCANCODE_A]) m_rdata.rdMoveRight -= 1;

        m_rdata.rdMoveUp = 0;
        if (keystate[SDL_SCANCODE_Q]) m_rdata.rdMoveUp += 1;
        if (keystate[SDL_SCANCODE_E]) m_rdata.rdMoveUp -= 1;

        // Update and render
        m_renderer.updateCameraFromRenderData(m_rdata, frameTime);
        m_renderer.renderFrame();
    }

    vkDeviceWaitIdle(*m_vkContext.device);
}

void Application::processEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                running = false;
                break;
            case SDLK_TAB:
                SDL_SetRelativeMouseMode(SDL_FALSE);
                SDL_ShowCursor(SDL_ENABLE);
                mouseLookEnabled = false;
                break;
            case SDLK_LSHIFT:
                SDL_SetRelativeMouseMode(SDL_TRUE);
                SDL_ShowCursor(SDL_DISABLE);
                mouseLookEnabled = true;
                break;
            default:
                break;
            }
            break;
        }

        case SDL_MOUSEMOTION:
        {
            float mouseSensitivity = 0.15f;
            if (mouseLookEnabled)
            {
                m_rdata.rdViewAzimuth += event.motion.xrel * mouseSensitivity;
                m_rdata.rdViewElevation -= event.motion.yrel * mouseSensitivity;
                m_rdata.rdViewElevation = std::clamp(m_rdata.rdViewElevation, -89.0f, 89.0f);
            }
            break;
        }

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                std::cout << "[Logger][Application] Changed window size to "
                          << event.window.data1 << " x " << event.window.data2 << '\n';

                m_rdata.rdWidth = event.window.data1;
                m_rdata.rdHeight = event.window.data2;
                vkDeviceWaitIdle(*m_vkContext.device);

                VkSwapchainKHR oldSwapchainHandle = m_swapchain ? m_swapchain->swapChain : VK_NULL_HANDLE;
                m_swapchain = std::make_shared<PyroCore::VulkanSwapchain>(m_rdata, m_vkContext, m_device, oldSwapchainHandle);
                m_renderer.recreateSwapchain(m_swapchain);
            }
            break;
        }
    }
}
