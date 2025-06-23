#include <iostream>
#include <vulkan/vulkan.h>
#include "tools/tools.hpp"
#include <glm.hpp>
#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_vulkan.h>

#define APPNAME "Vulkan renderer"

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Load Vulkan library
    if (SDL_Vulkan_LoadLibrary(nullptr) != 0) {
        std::cerr << "Failed to load Vulkan library: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create SDL window with Vulkan support
    SDL_Window* window = SDL_CreateWindow(
        APPNAME,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 360,
        SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
    );

    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return -1;
    }
	Logger::log(0,"SDL window and vulkan library successfully initialized");

    // Main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Rendering would go here

        SDL_Delay(16); // ~60 FPS
    }

    // Clean up
    SDL_DestroyWindow(window);
    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();

    return 0;
}