// Renderer.hpp
#pragma once

#include "window/window.hpp"
#include "engine/VulkanRenderdata.hpp"
#include "engine/VulkanLayerAndExtension.hpp"
#include "engine/VulkanInstance.hpp"
#include "engine/VulkanDebug.hpp"
#include "engine/VulkanDevice.hpp"
#include "engine/VulkanSwapchain.hpp"
#include <SDL.h>

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT;


class Renderer {
public:
    Renderer(VulkanRenderData& rData);
    ~Renderer();

    bool init();
    void run();
    void cleanup();


private:
    VulkanRenderData        renderData;
    window                  myWindow;
    VulkanLayerAndExtension vkLayersExt;
    VulkanInstance          vkInstance;
    VulkanDebug             vkDebug;
    VulkanDevice            vkDevice;
    VulkanSwapchain         vkSwapchain;

    bool running = false;

    void processEvents();
    void renderFrame();

    void createCommandPool();							// Create command pool
	void buildSwapChainAndDepthImage();					// Create swapchain color image and depth image
	void createDepthImage();							// Create depth image


    VkCommandPool		cmdPool;
	VkCommandBuffer		cmdDepthImage;	// Command buffer for depth image layout

	void destroyCommandBuffer();
	void destroyCommandPool();
	void destroyDepthBuffer();
    void setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer& cmd);
    
    struct{
		VkFormat		format;
		VkImage			image;
		VkDeviceMemory	mem;
		VkImageView		view;
	}Depth;




    //TESTING
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> framebuffers;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkCommandBuffer commandBuffer;

};
