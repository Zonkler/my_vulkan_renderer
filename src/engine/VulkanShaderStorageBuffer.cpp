#include "engine/VulkanShaderStorageBuffer.hpp"

#include <cstring>

bool ShaderStorageBuffer::init(VulkanRenderData& renderData, VmaAllocator allocator,VkShaderStorageBufferData &SSBOData, size_t bufferSize) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = bufferSize;
  bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

  VmaAllocationCreateInfo vmaAllocInfo{};
  vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  VkResult result = vmaCreateBuffer(allocator, &bufferInfo, &vmaAllocInfo,
    &SSBOData.buffer, &SSBOData.bufferAlloc, nullptr);
  if (result != VK_SUCCESS) {
    return false;
  }

  SSBOData.bufferSize = bufferSize;
    return true;
}

bool ShaderStorageBuffer::uploadSsboData(VulkanRenderData &renderData,VmaAllocator allocator ,VkShaderStorageBufferData &SSBOData, std::vector<glm::mat4> bufferData) {
  if (bufferData.empty()) {
    return false;
  }

  bool bufferResized = false;
  size_t bufferSize = bufferData.size() * sizeof(glm::mat4);
  if (bufferSize > SSBOData.bufferSize) {
    cleanup(renderData, allocator,SSBOData);
    init(renderData, allocator,SSBOData, bufferSize);
    bufferResized = true;
  }

  void* data;
  VkResult result = vmaMapMemory(allocator, SSBOData.bufferAlloc, &data);
  if (result != VK_SUCCESS) {
    return false;
  }
  std::memcpy(data, bufferData.data(), bufferSize);
  vmaUnmapMemory(allocator, SSBOData.bufferAlloc);
  vmaFlushAllocation(allocator, SSBOData.bufferAlloc, 0, SSBOData.bufferSize);

  return bufferResized;
}

bool ShaderStorageBuffer::checkForResize(VulkanRenderData& renderData,VmaAllocator allocator,VkShaderStorageBufferData& SSBOData, size_t bufferSize) {
  if (bufferSize > SSBOData.bufferSize) {
    cleanup(renderData,allocator ,SSBOData);
    init(renderData,allocator ,SSBOData, bufferSize);
    return true;
  }
  return false;
}

void ShaderStorageBuffer::cleanup(VulkanRenderData& renderData,VmaAllocator allocator ,VkShaderStorageBufferData &SSBOData) {
  vmaDestroyBuffer(allocator, SSBOData.buffer, SSBOData.bufferAlloc);
}
