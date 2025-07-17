#include <iostream>

#include <SDL.h>
#include <SDL_vulkan.h>

#include "window/window.hpp"
#include "tools/tools.hpp"
namespace PyroCore
{
    window::window(VulkanRenderData& rData){

        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "[WARNING][Logger][Window] Failed to initialize SDL: " << SDL_GetError() << std::endl;
            
        }

        // Load Vulkan library
        if (SDL_Vulkan_LoadLibrary(nullptr) != 0) {
            std::cerr << "[WARNING][Logger][Window] Failed to load Vulkan library: " << SDL_GetError() << std::endl;
        }

        // Create SDL window with Vulkan support
        rData.window = SDL_CreateWindow(
            rData.appname,
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            rData.rdWidth, rData.rdHeight,
            SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
        );
        
        if (!rData.window) {
            std::cerr << "[WARNING][Logger][Window] Failed to create window: " << SDL_GetError() << std::endl;
        }

        SDL_SetWindowResizable(rData.window,SDL_TRUE);
        m_window = rData.window;

        Logger::log(0,"[Logger][Window] SDL window ({0}x{1}) and vulkan library initialized\n",rData.rdWidth,rData.rdHeight);

    }
    window::~window(){

        SDL_DestroyWindow(m_window);
        SDL_Vulkan_UnloadLibrary();
        SDL_Quit();
        Logger::log(0,"[Logger][Window] Window destroyed\n");

    }

}