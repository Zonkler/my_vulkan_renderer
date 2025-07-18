#pragma once
#include <vulkan/vulkan.h>

#include <vector>
#include <array>


class VulkanQueue
{

public: 
    VulkanQueue(VkDevice& Device, VkSwapchainKHR swapchain, uint32_t queueFamily,uint32_t queueIndex,uint32_t swapchainImageCount);

    VulkanQueue(const VulkanQueue&)             = delete;
    VulkanQueue(VulkanQueue&&)                  = delete;
    VulkanQueue operator=(const VulkanQueue&)   = delete;
    VulkanQueue operator=(VulkanQueue&&)         = delete;

    ~VulkanQueue();

    void init(VkDevice& Device, VkSwapchainKHR swapchain, uint32_t queueFamily,uint32_t queueIndex,uint32_t swapchainImageCount);
    void destroy();
    uint32_t acquireNextImage();
    void submitAsync(VkCommandBuffer& cmdBuf,uint32_t imageIndex);
    void submitSync(VkCommandBuffer& cmdBuf);
    void present(uint32_t imageIndex);
    void waitIdle();
    void waitForCurrentFrameFence();

private:
    void createSemaphores();
    
    VkDevice * m_device = nullptr;
    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    VkQueue m_queue = VK_NULL_HANDLE;
    VkSemaphore m_renderCompleteSem;
    VkSemaphore m_presentCompleteSem;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    size_t currentFrame = 0;

    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT>   imageAvailableSemaphores;
    std::vector<VkSemaphore>                        renderFinishedSemaphores;
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT>       inFlightFences;
    std::vector<VkFence>                            imagesInFlight;
    std::vector<size_t>                             imageSemaphoreOwner;

};


