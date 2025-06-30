
#include "window/window.hpp"
#include "tools/tools.hpp"
#include <SDL.h>
#include <SDL_vulkan.h>
#include <iostream>
window::window(VulkanRenderData& rData)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        //return -1;
    }

    // Load Vulkan library
    if (SDL_Vulkan_LoadLibrary(nullptr) != 0) {
        std::cerr << "Failed to load Vulkan library: " << SDL_GetError() << std::endl;
        //return -1;
    }

    // Create SDL window with Vulkan support
    rData.window = SDL_CreateWindow(
        rData.appname,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        rData.rdHeight, rData.rdWidth,
        SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
    );
    
    if (!rData.window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        //return -1;
    }
    m_window = rData.window;
	
    Logger::log(0,"LOGGER::WINDOW:: SDL window and vulkan library initialized\n");





}

window::~window(){
    // Clean up
    Logger::log(0,"LOGGER::WINDOW:: Window destroyed\n");

    SDL_DestroyWindow(m_window);
    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();
}

/*
int window::gatherVulkanExtensions(VulkanRenderData& rData){

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

    Logger::log(0, "SDL and Vulkan extensions retrieved\n");

}
*/