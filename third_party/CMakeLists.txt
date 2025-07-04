# vkbootstrap
add_library(vkbootstrap STATIC)
target_sources(vkbootstrap PRIVATE vkbootstrap/VkBootstrap.cpp)
target_include_directories(vkbootstrap PUBLIC vkbootstrap)
target_link_libraries(vkbootstrap PUBLIC Vulkan::Vulkan)
set_property(TARGET vkbootstrap PROPERTY CXX_STANDARD 20)

# glm (header only)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)

# vma (header only)
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE vma)

# stb_image (header only)
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE stb_image)

# imgui
add_library(imgui STATIC)
target_include_directories(imgui PUBLIC imgui)
target_sources(imgui PRIVATE
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_impl_sdl2.cpp
    imgui/imgui_impl_vulkan.cpp
)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan SDL2::SDL2)

# fmt
add_subdirectory(fmt EXCLUDE_FROM_ALL)

# SDL2
add_subdirectory(SDL EXCLUDE_FROM_ALL)
