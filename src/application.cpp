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
        m_swapchain(m_rdata,m_vkContext,m_device),
        m_renderer(m_rdata,m_vkContext,m_swapchain.swapChain,m_swapchain)
{


};

void Application::run(){
    VkShaderStorageBufferData m_cameraSSBO;

    running = true;

    while (running)
    {
        processEvents();
        /* updateCameraPosition();
                std::cout << "CamPos: (" << m_rdata.rdCameraWorldPosition.x << ", "
                                << m_rdata.rdCameraWorldPosition.y << ", "
                                << m_rdata.rdCameraWorldPosition.z << ")  "
                  << "Yaw: " << m_rdata.rdViewAzimuth << " Pitch: " << m_rdata.rdViewElevation << "\r";

        // Upload updated camera matrix to GPU
        std::vector<glm::mat4> camData = { cameraMatrix };
        ShaderStorageBuffer::uploadSsboData(m_rdata, *m_vkContext.allocator, m_cameraSSBO, camData);
        */
        m_renderer.renderFrame();
    }
    vkDeviceWaitIdle(*m_vkContext.device);
    
}

void Application::processEvents(){
   // float cameraSpeed = 0.01f;
   // float mouseSensitivity = 0.2f;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
    switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        /*
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            bool pressed = (event.type == SDL_KEYDOWN);
            switch (event.key.keysym.sym) {
                case SDLK_w: m_rdata.rdMoveForward = pressed ? 1 : 0; break;
                case SDLK_s: m_rdata.rdMoveForward = pressed ? -1 : 0; break;
                case SDLK_a: m_rdata.rdMoveRight = pressed ? -1 : 0; break;
                case SDLK_d: m_rdata.rdMoveRight = pressed ? 1 : 0; break;
                case SDLK_SPACE: m_rdata.rdMoveUp = pressed ? 1 : 0; break;
                case SDLK_LCTRL: m_rdata.rdMoveUp = pressed ? -1 : 0; break;
                case SDLK_ESCAPE: running = true; break;
            }
            break;
        }
        case SDL_MOUSEMOTION: {
            m_rdata.rdViewAzimuth += event.motion.xrel * mouseSensitivity;
            m_rdata.rdViewElevation -= event.motion.yrel * mouseSensitivity;
            m_rdata.rdViewElevation = glm::clamp(m_rdata.rdViewElevation, -89.0f, 89.0f);
            break;
        }
    }*/
    }
}
}
/*
void Application::updateCameraPosition() {
    float yawRad = glm::radians(m_rdata.rdViewAzimuth);
    float pitchRad = glm::radians(m_rdata.rdViewElevation);
    float cameraSpeed = 0.01f;
    float mouseSensitivity = 0.2f;
    // Calculate forward/right/up vectors
    glm::vec3 forward = glm::normalize(glm::vec3(
        cos(pitchRad) * sin(yawRad),
        sin(pitchRad),
        cos(pitchRad) * cos(yawRad)
    ));
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    m_rdata.rdCameraWorldPosition += forward * (float)m_rdata.rdMoveForward * cameraSpeed;
    m_rdata.rdCameraWorldPosition += right * (float)m_rdata.rdMoveRight * cameraSpeed;
    m_rdata.rdCameraWorldPosition += up * (float)m_rdata.rdMoveUp * cameraSpeed;


    glm::vec3 cameraTarget = m_rdata.rdCameraWorldPosition + forward;
    glm::mat4 view = glm::lookAt(m_rdata.rdCameraWorldPosition, cameraTarget, glm::vec3(0, 1, 0));

    float aspect = (float)m_rdata.rdWidth / (float)m_rdata.rdHeight;
    glm::mat4 projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 100.0f);
    projection[1][1] *= -1.0f; // Vulkan correction

    cameraMatrix = projection * view;
}
    */