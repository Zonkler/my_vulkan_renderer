#pragma once
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vector>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>


struct VkVertex {
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec4 color = glm::vec4(1.0f);
  glm::vec3 normal = glm::vec3(0.0f);
  glm::vec2 uv = glm::vec2(0.0f);
  glm::uvec4 boneNumber = glm::uvec4(0);
  glm::vec4 boneWeight = glm::vec4(0.0f);
};

struct VkMesh {
  std::vector<VkVertex> vertices{};
  std::vector<uint32_t> indices{};
  //std::unordered_map<aiTextureType, std::string> textures{};
  bool usesPBRColors = false;
};

struct VkUploadMatrices {
  glm::mat4 viewMatrix{};
  glm::mat4 projectionMatrix{};
};

struct VkTextureData {
  VkImage image = VK_NULL_HANDLE;
  VkImageView imageView = VK_NULL_HANDLE;
  VkSampler sampler = VK_NULL_HANDLE;
  VmaAllocation imageAlloc = nullptr;

  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};

struct VkVertexBufferData {
  unsigned int bufferSize = 0;
  void* data = nullptr;
  VkBuffer buffer = VK_NULL_HANDLE;
  VmaAllocation bufferAlloc = VK_NULL_HANDLE;
  VkBuffer stagingBuffer = VK_NULL_HANDLE;
  VmaAllocation stagingBufferAlloc = VK_NULL_HANDLE;
};

struct VkIndexBufferData {
  size_t bufferSize = 0;
  VkBuffer buffer = VK_NULL_HANDLE;
  VmaAllocation bufferAlloc = nullptr;
  VkBuffer stagingBuffer = VK_NULL_HANDLE;
  VmaAllocation stagingBufferAlloc = nullptr;
};

struct VkUniformBufferData {
  size_t bufferSize = 0;
  VkBuffer buffer = VK_NULL_HANDLE;
  VmaAllocation bufferAlloc = nullptr;

  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};

struct VkShaderStorageBufferData {
  size_t bufferSize = 0;
  VkBuffer buffer = VK_NULL_HANDLE;
  VmaAllocation bufferAlloc = nullptr;

  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};

struct VkPushConstants {
  glm::mat4 transform{1.0f};
  alignas(16)glm::vec3 color;
};


struct VulkanRenderData {
    SDL_Window* window = nullptr;
    
    const char * appname = nullptr;
    
    uint32_t rdWidth = 0;
    
    uint32_t rdHeight = 0;
  
    std::vector<const char*> extensions;

    std::vector<const char*> layers;
  
  //unsigned int rdTriangleCount = 0;
  //unsigned int rdMatricesSize = 0;

  //std::vector<Light> Lights;
  //int rdLightIndex=0;
  //const int rdMaxLights=32;

  int rdFieldOfView = 60;

  //float rdFrameTime = 0.0f;
  //float rdMatrixGenerateTime = 0.0f;
  //float rdUploadToVBOTime = 0.0f;
  //float rdUploadToUBOTime = 0.0f;
  //float rdUIGenerateTime = 0.0f;
  //float rdUIDrawTime = 0.0f;

  int rdMoveForward = 0;
  int rdMoveRight = 0;
  int rdMoveUp = 0;

  float rdViewAzimuth = 330.0f;
  float rdViewElevation = -20.0f;
  glm::vec3 rdCameraWorldPosition = glm::vec3(2.0f, 5.0f, 7.0f);
};