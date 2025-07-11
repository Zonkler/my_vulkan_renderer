#include "engine/VulkanSwapchain.hpp"
#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include "engine/VulkanRenderdata.hpp"
#include "tools/tools.hpp"
#include "engine/VulkanDevice.hpp"
#include "engine/VulkanContext.hpp"
#include <vector>
#include <cassert>
#include <iostream>

VulkanSwapchain::VulkanSwapchain(const VulkanRenderData& rData, VulkanContext& vkContext, VulkanDevice& Device){
	m_instance = vkContext.instance;
	m_DeviceObj = &Device.device;

	auto result = SDL_Vulkan_CreateSurface(rData.window, *m_instance, &surface);
    if (!result)
    {
        Logger::log(0,"[ERROR][Logger][Swapchain] SDL failed to create surface\n");

    }

    auto index = getGraphicsQueueWithPresentationSupport(Device);
	
	Device.graphicsQueueWithPresentIndex = index;
	
    getSupportedFormats(Device);
    getSurfaceCapabilitiesAndPresentMode(Device,rData);
	printCapabilities();
	managePresentMode();
	createSwapChainColorBufferImages(Device);

	vkContext.swapchainExtent = &swapChainExtent;
	vkContext.SwapchainImageCnt = swapchainImageCount;
	vkContext.Format = format;
}

VulkanSwapchain::~VulkanSwapchain(){

	for (auto& buffer : colorBuffer) {
		if (buffer.view != VK_NULL_HANDLE) {
			vkDestroyImageView(*m_DeviceObj, buffer.view, nullptr);
			buffer.view = VK_NULL_HANDLE;

		}
	}
	Logger::log(0,"[Logger][Swapchain] Destroyed Swapchain Images\n");


	if (swapChain != VK_NULL_HANDLE) {
    	vkDestroySwapchainKHR(*m_DeviceObj, swapChain, nullptr);
		swapChain = VK_NULL_HANDLE;

	}
	Logger::log(0,"[Logger][Swapchain] Destroyed Swapchain\n");


	if (surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(*m_instance, surface, nullptr);
		surface = VK_NULL_HANDLE;
	}

    Logger::log(0,"[Logger][Swapchain] Destroyed surface\n");

}

uint32_t VulkanSwapchain::getGraphicsQueueWithPresentationSupport(const VulkanDevice& Device){
  
	uint32_t queueCount		= Device.queueFamilyCount;
	const VkPhysicalDevice& gpu	= Device.gpu;
	auto& queueProps        = Device.queueFamilyProps;
    
    std::vector<VkBool32> supportsPresent(queueCount);

    for (size_t i = 0; i < queueCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu,i,surface,&supportsPresent[i]);


    }
    	
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	uint32_t presentQueueNodeIndex = UINT32_MAX;
    for (size_t i = 0; i < queueCount; i++)
    {
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)!=0)
        {
            if (graphicsQueueNodeIndex == UINT32_MAX) {
				graphicsQueueNodeIndex = i;
			}

			if (supportsPresent[i] == VK_TRUE) {
				graphicsQueueNodeIndex = i;
				presentQueueNodeIndex = i;
				break;
			}
        }
        
    }
    
	if (presentQueueNodeIndex == UINT32_MAX) {
		// If didn't find a queue that supports both graphics and present, then
		// find a separate present queue.
		for (uint32_t i = 0; i < queueCount; ++i) {
			if (supportsPresent[i] == VK_TRUE) {
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
    
    // Generate error if could not find both a graphics and a present queue
	if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) {
		return  UINT32_MAX;
        Logger::log(0,"[ERROR][Logger][Swapchain] Could not find a graphics queue and a present queue\n");
	}
    Logger::log(0,"[Logger][Swapchain] Found a graphics queue and a present queue\n");
	return graphicsQueueNodeIndex;

}

void  VulkanSwapchain::getSupportedFormats(const VulkanDevice& Device){

    const VkPhysicalDevice gpu = Device.gpu;
	VkResult  result;

    // Get the list of VkFormats that are supported:
	uint32_t formatCount;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, NULL);
	assert(result == VK_SUCCESS);

	surfFormats.clear();
	surfFormats.resize(formatCount);

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, &surfFormats[0]);
	assert(result == VK_SUCCESS);

    // In case it’s a VK_FORMAT_UNDEFINED, then surface has no 
	// preferred format. We use BGRA 32 bit format
	if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		format = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		assert(formatCount >= 1);
		format = surfFormats[0].format;
	}
    
    Logger::log(0,"[Logger][Swapchain] Format: {}\n", static_cast<int>(format));
	
}

void VulkanSwapchain::getSurfaceCapabilitiesAndPresentMode(const VulkanDevice& device, const VulkanRenderData& rData){
	
	VkResult result;
	const VkPhysicalDevice &GPU = device.gpu;

	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GPU,surface,&surfCapabilities);
	assert(result == VK_SUCCESS);

	result = vkGetPhysicalDeviceSurfacePresentModesKHR(GPU,surface,&presentModeCount,nullptr);
	assert(result == VK_SUCCESS);

	presentModes.clear();
	presentModes.resize(presentModeCount);
	

	result = vkGetPhysicalDeviceSurfacePresentModesKHR(GPU, surface, &presentModeCount, presentModes.data());

	if (surfCapabilities.currentExtent.width == (uint32_t)-1)
	{
		// If the surface width and height is not defined, the set the equal to image size.
		swapChainExtent.width = rData.rdWidth;
		swapChainExtent.height = rData.rdHeight;
	}
	else
	{
		// If the surface size is defined, then it must match the swap chain size
		swapChainExtent = surfCapabilities.currentExtent;
	}



};

void VulkanSwapchain::printCapabilities(){
    Logger::log(0, "[Logger][Swapchain] Printing capabilities\n");
	
    std::cout << "\tCurrent extent: " << surfCapabilities.currentExtent.width 
              << "x" << surfCapabilities.currentExtent.height << '\n';

    std::cout << "\tMin image extent: " << surfCapabilities.minImageExtent.width 
              << "x" << surfCapabilities.minImageExtent.height << '\n';

    std::cout << "\tMax image extent: " << surfCapabilities.maxImageExtent.width 
              << "x" << surfCapabilities.maxImageExtent.height << '\n';

    std::cout << "\tMin image count: " << surfCapabilities.minImageCount << '\n';
    std::cout << "\tMax image count: " 
              << (surfCapabilities.maxImageCount == 0 ? "No maximum" 
                  : std::to_string(surfCapabilities.maxImageCount)) 
              << '\n';

    std::cout << "\tSupported transforms: 0x" 
              << std::hex << surfCapabilities.supportedTransforms 
              << std::dec << '\n';
    
    std::cout << "\tCurrent transform: 0x" 
              << std::hex << surfCapabilities.currentTransform 
              << std::dec << '\n';

    std::cout << "\tSupported composite alpha: 0x" 
              << std::hex << surfCapabilities.supportedCompositeAlpha 
              << std::dec << '\n';

    std::cout << "\tSupported usage flags: 0x" 
              << std::hex << surfCapabilities.supportedUsageFlags 
              << std::dec << '\n';


	std::cout << "\n\tSupported present modes:\n";
	for (const auto& mode : presentModes) {
		switch (mode) {
			case VK_PRESENT_MODE_IMMEDIATE_KHR:
				std::cout << "\t  VK_PRESENT_MODE_IMMEDIATE_KHR\n";
				break;
			case VK_PRESENT_MODE_MAILBOX_KHR:
				std::cout << "\t  VK_PRESENT_MODE_MAILBOX_KHR\n";
				break;
			case VK_PRESENT_MODE_FIFO_KHR:
				std::cout << "\t  VK_PRESENT_MODE_FIFO_KHR (guaranteed)\n";
				break;
			case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
				std::cout << "\t  VK_PRESENT_MODE_FIFO_RELAXED_KHR\n";
				break;
			case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
				std::cout << "\t  VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR\n";
				break;
			case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
				std::cout << "\t  VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR\n";
				break;
			default:
				std::cout << "\t  Unknown mode: " << mode << '\n';
				break;
		}
	}

}

void VulkanSwapchain::managePresentMode(){
	// If mailbox mode is available, use it, as is the lowest-latency non-
	// tearing mode.  If not, try IMMEDIATE which will usually be available,
	// and is fastest (though it tears).  If not, fall back to FIFO which is
	// always available.
	swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (size_t i = 0; i < presentModeCount; i++) {
		if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
			(presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
			swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	// Determine the number of VkImage's to use in the swap chain (we desire to
	// own only 1 image at a time, besides the images being displayed and
	// queued for display):
	desiredNumberOfSwapChainImages = surfCapabilities.minImageCount + 1;
	if ((surfCapabilities.maxImageCount > 0) &&
		(desiredNumberOfSwapChainImages > surfCapabilities.maxImageCount))
	{
		// Application must settle for fewer images than desired:
		desiredNumberOfSwapChainImages =surfCapabilities.maxImageCount;
	}

	if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else {
		preTransform = surfCapabilities.currentTransform;
	}

}

void VulkanSwapchain::createSwapChainColorBufferImages(const VulkanDevice& Device)
{
	VkSwapchainCreateInfoKHR scInfo{};

	scInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	scInfo.pNext = nullptr;
	scInfo.surface = surface;
	scInfo.minImageCount = desiredNumberOfSwapChainImages;
	scInfo.imageFormat = format;
	scInfo.imageExtent.height = swapChainExtent.height;
	scInfo.imageExtent.width = swapChainExtent.width;
	scInfo.preTransform = preTransform;
	scInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	scInfo.imageArrayLayers = 1;
	scInfo.presentMode = swapchainPresentMode;
	scInfo.oldSwapchain = VK_NULL_HANDLE;
	scInfo.clipped = true;
	scInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	scInfo.imageSharingMode		= VK_SHARING_MODE_EXCLUSIVE;
	scInfo.queueFamilyIndexCount = 0;
	scInfo.pQueueFamilyIndices	= NULL;

	VkResult result = vkCreateSwapchainKHR(Device.device,&scInfo,nullptr,&swapChain);
	

	// Create the swapchain object
	result = vkGetSwapchainImagesKHR(Device.device, swapChain, &swapchainImageCount, NULL);
	assert(result == VK_SUCCESS);

	swapchainImages.clear();
	// Get the number of images the swapchain has
	swapchainImages.resize(swapchainImageCount);

	// Retrieve the swapchain image surfaces 
	result = vkGetSwapchainImagesKHR(Device.device, swapChain, &swapchainImageCount, &swapchainImages[0]);
	Logger::log(0,"[Logger][Swapchain] Swapchain created\n");

}

void VulkanSwapchain::createColorImageView(const VkCommandBuffer& cmd,const VkDevice& Device){
	
	colorBuffer.clear();

	VkResult result;
	for (size_t i = 0; i < swapchainImageCount; i++)
	{
		SwapChainBuffer sc_buffer;

		VkImageViewCreateInfo imgViewInfo = {};

		imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imgViewInfo.pNext = NULL;
		imgViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		imgViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		imgViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		imgViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imgViewInfo.subresourceRange.baseMipLevel = 0;
		imgViewInfo.subresourceRange.levelCount = 1;
		imgViewInfo.subresourceRange.baseArrayLayer = 0;
		imgViewInfo.subresourceRange.layerCount = 1;
		imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imgViewInfo.flags = 0;
		imgViewInfo.format = format;

		sc_buffer.image = swapchainImages[i];
		imgViewInfo.image = sc_buffer.image;
		result = vkCreateImageView(Device, &imgViewInfo, NULL, &sc_buffer.view);
		colorBuffer.push_back(sc_buffer);
		assert(result == VK_SUCCESS);

	}
	
	currentColorBuffer = 0;

}
