// Renderer.hpp
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
    VulkanQueue             m_vkQueue;
    VulkanGFXPipeline       m_GFXPipeline;

    bool running = false;

    void processEvents();
    void renderFrame();

    void createCommandPool();							// Create command pool
	void buildSwapChainAndDepthImage();					// Create swapchain color image and depth image
	void createDepthImage();							// Create depth image


    VkCommandPool		cmdPool;
	VkCommandBuffer		cmdDepthImage;	// Command buffer for depth image layout
    VmaAllocator        allocator;

    
	void destroyCommandBuffer();
	void destroyCommandPool();
	void destroyDepthBuffer();
    void setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer& cmd);
    
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
    
    VkRenderPass createSimpleRenderPass(VkFormat format);
    std::vector<VkFramebuffer> createFrameBuffers(VkRenderPass);

};
