#pragma once

#include <SDL.h>
#include "engine/VulkanRenderdata.hpp"

class window
{
private:
    SDL_Window * m_window;
public:
    window(VulkanRenderData& rData);
    ~window();


};





