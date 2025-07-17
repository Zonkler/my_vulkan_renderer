#pragma once
#include <vulkan/vulkan.h>
#include "engine/VulkanRenderdata.hpp"
#include "engine/VulkanContext.hpp"
#include "engine/VulkanDevice.hpp"

struct SwapChainBuffer{
	VkImage image;
	VkImageView view;
};

namespace PyroCore
{
    


class VulkanSwapchain
{
public:

    VkSurfaceKHR surface;
    std::vector<VkSurfaceFormatKHR> surfFormats;
    VkFormat format;
    const VkInstance* m_instance;
    const VkDevice* m_DeviceObj;
    const VulkanRenderData* m_rData;
    // Store the image surface capabilities
	VkSurfaceCapabilitiesKHR	surfCapabilities;
	
    //arrays for retrived present modes
    std::vector<VkPresentModeKHR> presentModes;
    uint32_t presentModeCount;

    VkExtent2D swapChainExtent;


    // Swap chain object
	VkSwapchainKHR swapChain;

	// Stores present mode bitwise flag for the creation of swap chain
	VkPresentModeKHR			swapchainPresentMode;
	uint32_t					desiredNumberOfSwapChainImages;
	VkSurfaceTransformFlagBitsKHR preTransform;
    std::vector<VkImage>		swapchainImages;
    uint32_t swapchainImageCount;

    // List of color swap chain images
	std::vector<SwapChainBuffer> colorBuffer;
    // Current drawing surface index in use
	uint32_t currentColorBuffer;

    struct FrameSyncObjects {
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence;
    };
    
    std::vector<FrameSyncObjects> frameSyncObjects;
    uint32_t currentFrame = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;

     float extentAspectRatio() {
        return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
    }
    uint32_t getGraphicsQueueWithPresentationSupport(const PyroCore::VulkanDevice& Device);
    

public:
    VulkanSwapchain(const VulkanRenderData& rData,VulkanContext& vkContext, PyroCore::VulkanDevice& Device,VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);
    ~VulkanSwapchain();

    void getSupportedFormats(const PyroCore::VulkanDevice& Device);
    void getSurfaceCapabilitiesAndPresentMode(const PyroCore::VulkanDevice& device,const VulkanRenderData& rData);
    void printCapabilities();
    void managePresentMode();
    void createSwapChainColorBufferImages(const PyroCore::VulkanDevice& Device,VkSwapchainKHR oldSwapchain);
    void createColorImageView(const VkCommandBuffer& cmd,const VkDevice& Device);

};

} // namespace PyroCore