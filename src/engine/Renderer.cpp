// Renderer.cpp
#include "tools/tools.hpp"
#include "engine/Renderer.hpp"
#include <vulkan/vulkan.h>
#include <iostream>
#include <cassert>

#define VMA_IMPLEMENTATION

#include <vma/vk_mem_alloc.h>

#include "engine/Wrapper.hpp"


Renderer::Renderer(VulkanRenderData &rData) : renderData(rData)
{
	// Note: It's very important to initilize the member with 0 or respective value other wise it will break the system
	memset(&Depth, 0, sizeof(Depth));
}

Renderer::~Renderer()
{
	cleanup();
}

bool Renderer::init()
{


	if (!myWindow.init(renderData))
	{
		Logger::log(0, "[WARNING][Logger][Window] Window initialization failed\n");
		return false;
	}

	if (!vkLayersExt.init(renderData))
	{
		Logger::log(0, "[WARNING][Logger][Vulkan layer & extension] Failed retrieving vulkan extensions\n");
		return false;
	}

	if (!vkInstance.init(renderData))
	{
		Logger::log(0, "[WARNING][Logger][Vulkan Instance] Creation of Vulkan instance failed\n");
		return false;
	}

	if (!vkDebug.init(vkInstance.Get_VKinstance()))
	{
		Logger::log(0, "[WARNING][Logger][Debug] Creation of debugger failed\n");
		return false;
	}

	if (!vkDevice.init(vkInstance.Get_VKinstance(), {VK_KHR_SWAPCHAIN_EXTENSION_NAME}))
	{
		Logger::log(0, "[WARNING][Logger][Device] Creation of Logical device failed\n");
		return false;
	}

	allocator = vkDevice.getAllocator();

	vkSwapchain.init(renderData, vkInstance.Get_VKinstance(), vkDevice /*, cmdDepthImage*/);

	createCommandPool();
	createDepthImage();
	buildSwapChainAndDepthImage();

	m_vkQueue.init(vkDevice.device, vkSwapchain.swapChain, vkSwapchain.getGraphicsQueueWithPresentationSupport(vkDevice), 0, vkSwapchain.swapchainImageCount);

	m_cmdBuffers.resize(vkSwapchain.swapchainImageCount);
	
	m_renderPass = createSimpleRenderPass(vkSwapchain.format);
	m_frameBuffers = createFrameBuffers(m_renderPass);

	CommandBufferMgr::allocCommandBuffer(&vkDevice.device, cmdPool, m_cmdBuffers.data(), nullptr, m_cmdBuffers.size());

	VkClearColorValue Clearcolor = {0.3f, 0.3f, 0.3f, 0.3f};
	VkClearValue clearValue;
	clearValue.color = Clearcolor;

	VkImageSubresourceRange ImageRange = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};
	
	VkRenderPassBeginInfo RenderPassBeginInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = m_renderPass,
		.renderArea = {
			.offset = {
				.x=0,
				.y=0
			},
			.extent = {
				.width = static_cast<uint32_t>(renderData.rdWidth),
				.height = static_cast<uint32_t>(renderData.rdHeight)
			}
		},
		.clearValueCount =1,
		.pClearValues = &clearValue
	};

	
	for (size_t i = 0; i < m_cmdBuffers.size(); i++)
	{
		CommandBufferMgr::beginCommandBuffer(m_cmdBuffers[i], nullptr);

		RenderPassBeginInfo.framebuffer = m_frameBuffers[i];
		
		vkCmdBeginRenderPass(m_cmdBuffers[i],&RenderPassBeginInfo,VK_SUBPASS_CONTENTS_INLINE);

		vkCmdEndRenderPass(m_cmdBuffers[i]);
		CommandBufferMgr::endCommandBuffer(m_cmdBuffers[i]);
	}

	return true;
}

void Renderer::processEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
	
		if (event.type == SDL_QUIT)
		{
			running = false;
		}
	}
}

void Renderer::renderFrame()
{
	m_vkQueue.waitForCurrentFrameFence();

	uint32_t imageIndex = m_vkQueue.acquireNextImage();

	m_vkQueue.submitAsync(m_cmdBuffers[imageIndex], imageIndex);

	m_vkQueue.present(imageIndex);

	SDL_Delay(16);
	
}

void Renderer::run()
{
	running = true;
	while (running)
	{
		processEvents();
		renderFrame();
	}
}

void Renderer::cleanup()
{
	if (vkDevice.device)
	{
		vkDeviceWaitIdle(vkDevice.device);
	}
	destroyDepthBuffer();
	vkSwapchain.destroy();
	m_vkQueue.destroy();
	destroyCommandBuffer();
	destroyCommandPool();

	vkDevice.destroy();
	vkDebug.destroy();
	vkInstance.destroy();
	myWindow.destroy();
}

void Renderer::createCommandPool()
{
	VulkanDevice &deviceObj = vkDevice;
	/* Depends on intializeSwapChainExtension() */
	VkResult res;

	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.pNext = NULL;
	cmdPoolInfo.queueFamilyIndex = deviceObj.graphicsQueueFamilyIndex;
	cmdPoolInfo.flags = 0;

	res = vkCreateCommandPool(deviceObj.device, &cmdPoolInfo, NULL, &cmdPool);
	Logger::log(0, "[Logger][Renderer] Command pool created\n");
	// assert(res == VK_SUCCESS);
}

void Renderer::destroyCommandPool()
{
	VulkanDevice &deviceObj = vkDevice;

	vkDestroyCommandPool(deviceObj.device, cmdPool, NULL);

	Logger::log(0, "[Logger][Renderer] Command pool destroyed\n");
}

void Renderer::destroyCommandBuffer()
{
	VkCommandBuffer cmdBufs[] = {cmdDepthImage};
	vkFreeCommandBuffers(vkDevice.device, cmdPool, sizeof(cmdBufs) / sizeof(VkCommandBuffer), cmdBufs);
}

void Renderer::setImageLayout(
	VkImage image,
	VkImageAspectFlags aspectMask,
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout,
	VkAccessFlagBits srcAccessMask,
	const VkCommandBuffer &cmd)
{
	assert(cmd != VK_NULL_HANDLE);

	VkImageMemoryBarrier imgMemoryBarrier = {};
	imgMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imgMemoryBarrier.pNext = NULL;
	imgMemoryBarrier.srcAccessMask = srcAccessMask;
	imgMemoryBarrier.dstAccessMask = 0;
	imgMemoryBarrier.oldLayout = oldImageLayout;
	imgMemoryBarrier.newLayout = newImageLayout;
	imgMemoryBarrier.image = image;
	imgMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	imgMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imgMemoryBarrier.subresourceRange.levelCount = 1;
	imgMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imgMemoryBarrier.subresourceRange.layerCount = 1;

	if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		imgMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	switch (newImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imgMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		// No access mask needed for presenting images
		imgMemoryBarrier.dstAccessMask = 0;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		imgMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imgMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imgMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		imgMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	default:
		// Add other cases if needed
		break;
	}

	VkPipelineStageFlags srcStages = 0;
	VkPipelineStageFlags destStages = 0;

	switch (oldImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		srcStages = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		srcStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		srcStages = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		srcStages = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		break;
	default:
		srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		break;
	}

	switch (newImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		destStages = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		destStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		destStages = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		destStages = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		destStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		break;
	default:
		destStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		break;
	}

	vkCmdPipelineBarrier(
		cmd,
		srcStages,
		destStages,
		0,
		0, nullptr,
		0, nullptr,
		1, &imgMemoryBarrier);
}

void Renderer::createDepthImage()
{
    // Depth format fallback
    if (Depth.format == VK_FORMAT_UNDEFINED)
    {
        Depth.format = VK_FORMAT_D16_UNORM;
    }
    const VkFormat depthFormat = Depth.format;

    // Check format support
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(vkDevice.gpu, depthFormat, &props);
    if (!(props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
    {
        std::cerr << "Unsupported Depth Format for optimal tiling\n";
        exit(-1);
    }

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = depthFormat;
    imageInfo.extent.width = vkSwapchain.swapChainExtent.width;
    imageInfo.extent.height = vkSwapchain.swapChainExtent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = NUM_SAMPLES; // e.g. VK_SAMPLE_COUNT_1_BIT
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // Setup VMA allocation info - GPU-only memory (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    // Create image and allocate memory with VMA
    VkResult result = vmaCreateImage(
        allocator,   // Your VMA allocator instance
        &imageInfo,
        &allocCreateInfo,
        &Depth.image,
        &Depth.allocation,       // Note: You need to add this member: VmaAllocation Depth.allocation;
        nullptr);
    assert(result == VK_SUCCESS);

    // Create the image view as before
    VkImageViewCreateInfo imgViewInfo = {};
    imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgViewInfo.image = Depth.image;
    imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imgViewInfo.format = depthFormat;
    imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imgViewInfo.subresourceRange.baseMipLevel = 0;
    imgViewInfo.subresourceRange.levelCount = 1;
    imgViewInfo.subresourceRange.baseArrayLayer = 0;
    imgViewInfo.subresourceRange.layerCount = 1;
    imgViewInfo.flags = 0;

    // Add stencil aspect if needed
    if (depthFormat == VK_FORMAT_D16_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT)
    {
        imgViewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    result = vkCreateImageView(vkDevice.device, &imgViewInfo, nullptr, &Depth.view);
    assert(result == VK_SUCCESS);

    // Use a command buffer to transition the image layout to DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    CommandBufferMgr::allocCommandBuffer(&vkDevice.device, cmdPool, &cmdDepthImage, nullptr, 1);
    CommandBufferMgr::beginCommandBuffer(cmdDepthImage);
    {
        setImageLayout(Depth.image,
                       imgViewInfo.subresourceRange.aspectMask,
                       VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                       (VkAccessFlagBits)0,
                       cmdDepthImage);
    }
    CommandBufferMgr::endCommandBuffer(cmdDepthImage);
    CommandBufferMgr::submitCommandBuffer(vkDevice.queue, &cmdDepthImage);

    Logger::log(0, "[Logger][Renderer] Depth Buffer created with VMA\n");
}

void Renderer::buildSwapChainAndDepthImage()
{
	vkSwapchain.createColorImageView(cmdDepthImage, vkDevice);
}

void Renderer::destroyDepthBuffer()
{
    // Destroy image view
    vkDestroyImageView(vkDevice.device, Depth.view, nullptr);

    // Destroy image and free memory using VMA
    vmaDestroyImage(allocator, Depth.image, Depth.allocation);

    Logger::log(0, "[Logger][Renderer] Depth Buffer destroyed (VMA)\n");
}


VkRenderPass Renderer::createSimpleRenderPass(VkFormat format) {
    VkAttachmentDescription attachDesc = {
        .flags = 0,
        .format = format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference attachRef = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  // ✅ FIXED
    };

    VkSubpassDescription subpassDesc = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachRef
    };

    VkRenderPassCreateInfo renderPassCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &attachDesc,
        .subpassCount = 1,
        .pSubpasses = &subpassDesc
    };

    VkRenderPass renderPass;
    VkResult res = vkCreateRenderPass(vkDevice.device, &renderPassCreateInfo, nullptr, &renderPass);
    if (res != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }

    std::cout << "Created simple render pass\n";
    return renderPass;
}

std::vector<VkFramebuffer> Renderer::createFrameBuffers(VkRenderPass renderPass){

	m_frameBuffers.resize(vkSwapchain.swapchainImageCount);

	for (size_t i = 0; i < m_frameBuffers.size(); i++)
	{
		VkFramebufferCreateInfo fbCreateInfo{};

		fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbCreateInfo.renderPass = renderPass;
		fbCreateInfo.attachmentCount = 1;
		fbCreateInfo.pAttachments = &vkSwapchain.colorBuffer[i].view;
		fbCreateInfo.width = renderData.rdWidth;
		fbCreateInfo.height = renderData.rdHeight;
		fbCreateInfo.layers = 1;

		vkCreateFramebuffer(vkDevice.device,&fbCreateInfo,nullptr,&m_frameBuffers[i]);

	}
	
	return m_frameBuffers;

}