
#pragma once

#include "window/window.hpp"
#include <memory>
#include "engine/VulkanRenderdata.hpp"
#include "engine/VulkanLayerAndExtension.hpp"
#include "engine/VulkanInstance.hpp"
#include "engine/VulkanDebug.hpp"
#include "engine/VulkanDevice.hpp"
#include "engine/VulkanSwapchain.hpp"
#include "engine/VulkanQueue.hpp"
#include "engine/Shader.hpp"
#include "engine/VulkanGFXPipeline.hpp"

#include "engine/VulkanContext.hpp"
#include <SDL.h>

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT;


class Renderer {
public:
    Renderer(VulkanRenderData &rData, VulkanContext& vkContaxt,VkSwapchainKHR& swapchain,VulkanSwapchain& vkSwap);
    ~Renderer();

    void run();


public:
    VulkanRenderData&        renderData;
    VulkanSwapchain& vkSwapchain;
    VulkanQueue             m_vkQueue;
    VulkanGFXPipeline       m_GFXPipeline;

    bool running = false;

    void processEvents();
    void renderFrame();

    void createCommandPool();							// Create command pool
	void buildSwapChainAndDepthImage();					// Create swapchain color image and depth image
	void createDepthImage();							// Create depth image
    void recordCommandbuffers();

    VkCommandPool		cmdPool;
	VkCommandBuffer		cmdDepthImage;	// Command buffer for depth image layout
    VmaAllocator*        allocator;

    
	void destroyCommandBuffer();
	void destroyCommandPool();
	void destroyDepthBuffer();
    
    struct{
		VkFormat		format;
		VkImage			image;
		VkDeviceMemory	mem;
		VkImageView		view;
        VmaAllocation   allocation;
	}Depth;

    std::vector<VkCommandBuffer> m_cmdBuffers;
    std::vector<VkFramebuffer> m_frameBuffers;
    VkRenderPass m_renderPass;


    std::vector<std::unique_ptr<Shader>> ShaderModules;
    
    VulkanContext& m_vkContext;

    

};

