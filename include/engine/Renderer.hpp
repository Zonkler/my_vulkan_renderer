
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
#include "engine/VulkanVertexBuffer.hpp"
#include "engine/VulkanContext.hpp"
#include "model/gameobject.hpp"
#include "tools/camera.hpp"
#include <SDL.h>

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT;

namespace PyroCore
{
    
class Renderer {
public:
    Renderer(VulkanRenderData &rData, VulkanContext& vkContaxt,std::shared_ptr<PyroCore::VulkanSwapchain> swapchain);
    ~Renderer();
   

    void recreateSwapchain(std::shared_ptr<PyroCore::VulkanSwapchain>& swapchain);
    void run();

    void updateCameraFromRenderData(const VulkanRenderData& rdata, float frameTime);

public:
    VulkanRenderData&        renderData;
    std::shared_ptr<PyroCore::VulkanSwapchain> vkSwapchain;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::unique_ptr<VulkanQueue> m_vkQueue;
    std::unique_ptr<VulkanGFXPipeline> m_GFXPipeline;
    std::shared_ptr<Model>         triangle;
    Camera m_camera;
    bool running = false;

    void processEvents();
    void renderFrame();

    void createCommandPool();							// Create command pool
	void buildSwapChainAndDepthImage();					// Create swapchain color image and depth image
	void createDepthImage();							// Create depth image
    void recordCommandbuffers(uint32_t index);

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
    std::vector<gameobject> gameObjects;
    VulkanContext& m_vkContext;

    

};

} // namespace PyroCore