#pragma once

#include <SDL.h>
#include "engine/VulkanRenderdata.hpp"

/*
    NOTE:  
    Non-copyable, non-movable: Creates a window for the Vulkan application to use.
    It copies the m_window pointer to the rData struct.
*/
namespace PyroCore
{

    class window
    {
    private:
        SDL_Window * m_window;
    public:
        window(VulkanRenderData& rData);

        window(const window&)           = delete;
        window(window&&)          = delete;
        window& operator=(const window&)= delete;
        window& operator=(window&&)     = delete;
        
        ~window();

    };

} // namespace PyroCore





