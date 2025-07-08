#pragma once

#include <vulkan/vulkan.h>

#include <vma/vk_mem_alloc.h>

struct VulkanContext{
    VkInstance* instance;
    VkPhysicalDevice* pDevice;
    VkDevice* device;
    VmaAllocator* allocator;
    VkQueue queue;

    uint32_t GraphicsQueueWithPresentationSupport;
    uint32_t graphicsQueueFamilyIndex;
    uint32_t SwapchainImageCnt;
    VkFormat Format;
    VkRenderPass renderPass;
    VkExtent2D* swapchainExtent;

};