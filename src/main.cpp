#include <iostream>
#include <vulkan/vulkan.h>
#include "tools/tools.hpp"
#include <glm/glm.hpp>
#include <vector>
#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_vulkan.h>
#include "window/window.hpp"
#include "engine/VulkanRenderdata.hpp"
#include "engine/VulkanLayerAndExtension.hpp"
#include "engine/VulkanInstance.hpp"
#include "engine/VulkanDevice.hpp"
#include "engine/VulkanDebug.hpp"
#include <thread>

#define DEBUG

int main() {

    std::vector<const char *> deviceExtensionNames = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VulkanRenderData VKrenderData{};
    VKrenderData.rdHeight=640;
    VKrenderData.rdWidth=640;
    VKrenderData.appname = "MyVKRenderer";
    #ifdef DEBUG

    #endif

    window MyWindow(VKrenderData);
    VulkanLayerAndExtension VKlayernExt(VKrenderData);
    VulkanInstance VKinstance(VKrenderData);
    VulkanDebug VKDebug(VKinstance.Get_VKinstance());

    VulkanDevice VKdevice(VKinstance.Get_VKinstance(),deviceExtensionNames);

    /*
    this is for triggering an error to test the debugger
    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface(VKrenderData.window, VKinstance.Get_VKinstance(), &surface);
    vkDestroyInstance(VKinstance.Get_VKinstance(), nullptr);
    */



    // Main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Rendering 

        SDL_Delay(16); // 60 FPS
    }
    
    return 0;
}