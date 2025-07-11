/* Vulkan shader storage buffer object */
#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "engine/VulkanRenderdata.hpp"

class ShaderStorageBuffer {
  public:
    /* set an arbitraty buffer size as default */
    static bool init(VulkanRenderData &renderData,VmaAllocator allocator ,VkShaderStorageBufferData &SSBOData, size_t bufferSize = 1024);
    
    static bool uploadSsboData(VulkanRenderData &renderData,VmaAllocator allocator, VkShaderStorageBufferData &SSBOData,
      std::vector<glm::mat4> bufferData);

    static bool checkForResize(VulkanRenderData &renderData,VmaAllocator allocator, VkShaderStorageBufferData &SSBOData,
      size_t bufferSize);

    static void cleanup(VulkanRenderData &renderData,VmaAllocator allocator, VkShaderStorageBufferData &SSBOData);
};