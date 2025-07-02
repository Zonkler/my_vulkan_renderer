#define SDL_MAIN_HANDLED

#include "engine/Renderer.hpp"
#include "engine/VulkanRenderdata.hpp"
#include "tools/tools.hpp"
#include <cassert>
int main() {

    VulkanRenderData renderData{};

    renderData.rdWidth = 1280;
    renderData.rdHeight = 720; //720
    
    renderData.appname = "Magma Core";

    
    Renderer renderer(renderData);
    if (!renderer.init()) {
        return -1;
    }
    renderer.run();
    return 0;
}
