#pragma once
#include <vulkan/vulkan.h>

#include <vector>

class VulkanQueue
{
private:
    void createSemaphores();
    
    VkDevice * m_device = nullptr;
    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    VkQueue m_queue = VK_NULL_HANDLE;
    VkSemaphore m_renderCompleteSem;
    VkSemaphore m_presentCompleteSem;

public:
    VulkanQueue() = default;
    ~VulkanQueue()= default;

    void init(VkDevice& Device, VkSwapchainKHR swapchain, uint32_t queueFamily,uint32_t queueIndex);
    void destroy();
    uint32_t acquireNextImage();
    void submitAsync(VkCommandBuffer& cmdBuf);
    void submitSync(VkCommandBuffer& cmdBuf);
    void present(uint32_t imageIndex);
    void waitIdle();
};


