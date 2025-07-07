cmake_minimum_required(VERSION 3.16)
project(vulkan_guide)

find_package(Vulkan REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-march=native)
endif()

add_subdirectory(third_party)

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
)

# Add resource file here
if(WIN32)
    set(RESOURCE_FILE "${PROJECT_SOURCE_DIR}/resource.rc")
else()
    set(RESOURCE_FILE "")  # no resource on Linux/macOS
endif()

add_executable(vulkan_guide ${SOURCES} ${HEADERS} ${RESOURCE_FILE})

set_property(TARGET vulkan_guide PROPERTY CXX_STANDARD 20)
target_compile_definitions(vulkan_guide PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)

target_include_directories(vulkan_guide PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/third_party"
)

target_link_libraries(vulkan_guide PUBLIC
    vma glm Vulkan::Vulkan fmt::fmt stb_image SDL2::SDL2 vkbootstrap imgui
)

# Static link libgcc and libstdc++ to avoid runtime DLL issues with MinGW
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(vulkan_guide PRIVATE -static-libgcc -static-libstdc++)
    if (CMAKE_VERSION VERSION_LESS "3.13")
        # For older CMake, append to linker flags globally
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
    else()
        target_link_options(vulkan_guide PRIVATE -static-libgcc -static-libstdc++)
    endif()
endif()

target_precompile_headers(vulkan_guide PUBLIC 
    <optional> <vector> <memory> <string> <unordered_map>
    <glm/mat4x4.hpp> <glm/vec4.hpp> <vulkan/vulkan.h>
)

add_custom_command(TARGET vulkan_guide POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:vulkan_guide> $<TARGET_FILE_DIR:vulkan_guide>
    COMMAND_EXPAND_LISTS
)
