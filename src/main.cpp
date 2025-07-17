#define SDL_MAIN_HANDLED

//#include "engine/Renderer.hpp"
#include "Application.hpp"
#include "engine/VulkanRenderdata.hpp"
#include "tools/tools.hpp"
#include <cassert>

int main() {

    
    VulkanRenderData renderData{};

    renderData.rdWidth = 1280;
    renderData.rdHeight = 720; //720
    
    renderData.appname = "PyroCore";

    Application app(renderData);
    
    app.run();
    
    return 0;
}
