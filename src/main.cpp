// main.cpp
#define SDL_MAIN_HANDLED

#include "engine/Renderer.hpp"
#include "engine/VulkanRenderdata.hpp"
#include "tools/tools.hpp"
#include <cassert>
int main() {
    VulkanRenderData renderData{};
    // set initial renderData parameters
    renderData.rdHeight = 1280;
    renderData.rdWidth = 720;
    renderData.appname = "Magma Core";
    Logger::log(0,"Hi {} {}\n","SKIBIDIIII", "WORLD");
    Renderer renderer(renderData);
    if (!renderer.init()) {
        return -1;
    }
    renderer.run();
    return 0;
}
