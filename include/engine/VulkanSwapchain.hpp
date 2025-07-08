#pragma once
#include <vulkan/vulkan.h>
#include "engine/VulkanRenderdata.hpp"
#include "engine/VulkanContext.hpp"
#include "engine/VulkanDevice.hpp"

struct SwapChainBuffer{
	VkImage image;
	VkImageView view;
};


class VulkanSwapchain
{
public:
    VkSurfaceKHR surface;
    std::vector<VkSurfaceFormatKHR> surfFormats;
    VkFormat format;
    const VkInstance* m_instance;
    const VkDevice* m_DeviceObj;

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


    uint32_t getGraphicsQueueWithPresentationSupport(const VulkanDevice& Device);


public:
    VulkanSwapchain(const VulkanRenderData& rData,VulkanContext& vkContext, VulkanDevice& Device);
    ~VulkanSwapchain();
    int init(const VulkanRenderData& rData, const VkInstance& Instance, VulkanDevice& Device/*,const VkCommandBuffer& cmd*/);
    //void destroy();

    void getSupportedFormats(const VulkanDevice& Device);
    void getSurfaceCapabilitiesAndPresentMode(const VulkanDevice& device,const VulkanRenderData& rData);
    void printCapabilities();
    void managePresentMode();
    void createSwapChainColorBufferImages(const VulkanDevice& Device);
    void createColorImageView(const VkCommandBuffer& cmd,const VkDevice& Device);

};

