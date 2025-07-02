// Renderer.cpp
#include "tools/tools.hpp"
#include "engine/Renderer.hpp"
#include <vulkan/vulkan.h>
#include <iostream>
#include <cassert>
#include "engine/Wrapper.hpp"

Renderer::Renderer(VulkanRenderData& rData) : renderData(rData){
    // Note: It's very important to initilize the member with 0 or respective value other wise it will break the system
	memset(&Depth, 0, sizeof(Depth));
}

Renderer::~Renderer(){
    cleanup();
}

bool Renderer::init(){

	if(!myWindow.init(renderData)){
		Logger::log(0,"[WARNING][Logger][Window] Window initialization failed\n");
		return false;
	}
	
	if(!vkLayersExt.init(renderData)){
		Logger::log(0,"[WARNING][Logger][Vulkan layer & extension] Failed retrieving vulkan extensions\n");
		return false;
	}

	if(!vkInstance.init(renderData)){
		Logger::log(0,"[WARNING][Logger][Vulkan Instance] Creation of Vulkan instance failed\n");
		return false;
	}

	if(!vkDebug.init(vkInstance.Get_VKinstance())){
		Logger::log(0,"[WARNING][Logger][Debug] Creation of debugger failed\n");
		return false;
	}

	if(!vkDevice.init(vkInstance.Get_VKinstance(), {VK_KHR_SWAPCHAIN_EXTENSION_NAME})){
		Logger::log(0,"[WARNING][Logger][Device] Creation of Logical device failed\n");
		return false;
	}

	vkSwapchain.init(renderData, vkInstance.Get_VKinstance(), vkDevice /*, cmdDepthImage*/);

    createCommandPool();
    createDepthImage();
    buildSwapChainAndDepthImage();

	m_vkQueue.init(vkDevice.device,vkSwapchain.swapChain,vkSwapchain.getGraphicsQueueWithPresentationSupport(vkDevice),0);


	m_cmdBuffers.resize(vkSwapchain.swapchainImageCount);
	std::cout<<m_cmdBuffers.size()<<"\n";

	CommandBufferMgr::allocCommandBuffer(&vkDevice.device,cmdPool,m_cmdBuffers.data(),nullptr,m_cmdBuffers.size());
	
	VkClearColorValue ClearValue = {0.3f,0.3f,0.3f,0.3f};

		VkImageSubresourceRange ImageRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};


for (size_t i = 0; i < m_cmdBuffers.size(); i++)
{
	CommandBufferMgr::beginCommandBuffer(m_cmdBuffers[i],nullptr);

	setImageLayout(
    vkSwapchain.swapchainImages[i],
    VK_IMAGE_ASPECT_COLOR_BIT,
    VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    VK_ACCESS_NONE, // or VK_ACCESS_TRANSFER_WRITE_BIT if unclear
    m_cmdBuffers[i]
	);

	vkCmdClearColorImage(
		m_cmdBuffers[i],
		vkSwapchain.swapchainImages[i],
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		&ClearValue,
		1,
		&ImageRange);

	setImageLayout(
    vkSwapchain.swapchainImages[i],
    VK_IMAGE_ASPECT_COLOR_BIT,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    VK_ACCESS_TRANSFER_WRITE_BIT,
    m_cmdBuffers[i]
	);


	CommandBufferMgr::endCommandBuffer(m_cmdBuffers[i]);
}
	std::cout<<"Allocated memory\n";


    return true;
}

void Renderer::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }
}

void Renderer::renderFrame() {
	//std::cout<<"starting render frame\n";

	uint32_t ImageIndex = m_vkQueue.acquireNextImage();

	//std::cout<<"got index next image\n";


	m_vkQueue.submitAsync(m_cmdBuffers[ImageIndex]);

	//std::cout<<"submitted cmd buffer\n";


	m_vkQueue.present(ImageIndex);

		//std::cout<<"submitted cmd buffer to present\n";


    SDL_Delay(16);
}

void Renderer::run() {
    running = true;
    while (running) {
        processEvents();
        renderFrame();
    }
}

void Renderer::cleanup() {
	if (vkDevice.device){
		vkDeviceWaitIdle(vkDevice.device);
	}
	destroyDepthBuffer();           
	vkSwapchain.destroy();         
	destroyCommandBuffer();        
	destroyCommandPool();          

	vkDevice.destroy();            
	vkDebug.destroy();             
	vkInstance.destroy();           
	myWindow.destroy();             
}

void Renderer::createCommandPool()
{
	VulkanDevice& deviceObj		= vkDevice;
	/* Depends on intializeSwapChainExtension() */
	VkResult  res;

	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.pNext = NULL;
	cmdPoolInfo.queueFamilyIndex = deviceObj.graphicsQueueFamilyIndex;
	cmdPoolInfo.flags = 0;

	res = vkCreateCommandPool(deviceObj.device, &cmdPoolInfo, NULL, &cmdPool);
    Logger::log(0,"[Logger][Renderer] Command pool created\n");
    //assert(res == VK_SUCCESS);
}

void Renderer::destroyCommandPool()
{
	VulkanDevice& deviceObj		= vkDevice;

	vkDestroyCommandPool(deviceObj.device, cmdPool, NULL);
    
    Logger::log(0,"[Logger][Renderer] Command pool destroyed\n");

}

void Renderer::destroyCommandBuffer()
{
	VkCommandBuffer cmdBufs[] = { cmdDepthImage };
	vkFreeCommandBuffers(vkDevice.device, cmdPool, sizeof(cmdBufs)/sizeof(VkCommandBuffer), cmdBufs);
}

void Renderer::setImageLayout(
    VkImage image,
    VkImageAspectFlags aspectMask,
    VkImageLayout oldImageLayout,
    VkImageLayout newImageLayout,
    VkAccessFlagBits srcAccessMask,
    const VkCommandBuffer& cmd)
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

    if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
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

    switch (oldImageLayout) {
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

    switch (newImageLayout) {
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
        1, &imgMemoryBarrier
    );
}



void Renderer::createDepthImage()
{
	VkResult  result;
	bool  	  pass;

	VkImageCreateInfo imageInfo = {};

	// If the depth format is undefined, use fallback as 16-byte value
	if (Depth.format == VK_FORMAT_UNDEFINED) {
		Depth.format = VK_FORMAT_D16_UNORM;
	}

	const VkFormat depthFormat = Depth.format;

	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(vkDevice.gpu, depthFormat, &props);
	if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	}
	else if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
		imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
	}
	else {
		std::cout << "Unsupported Depth Format, try other Depth formats.\n";
		exit(-1);
	}

	imageInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext			= NULL;
	imageInfo.imageType		= VK_IMAGE_TYPE_2D;
	imageInfo.format		= depthFormat;
	imageInfo.extent.width	= vkSwapchain.swapChainExtent.width;
	imageInfo.extent.height = vkSwapchain.swapChainExtent.height;
	imageInfo.extent.depth	= 1;
	imageInfo.mipLevels		= 1;
	imageInfo.arrayLayers	= 1;
	imageInfo.samples		= NUM_SAMPLES;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices	= NULL;
	imageInfo.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.usage					= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.flags					= 0;

	// User create image info and create the image objects
	result = vkCreateImage(vkDevice.device, &imageInfo, NULL, &Depth.image);
	assert(result == VK_SUCCESS);

	// Get the image memory requirements
	VkMemoryRequirements memRqrmnt;
	vkGetImageMemoryRequirements(vkDevice.device,	Depth.image, &memRqrmnt);

	VkMemoryAllocateInfo memAlloc = {};
	memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAlloc.pNext = NULL;
	memAlloc.allocationSize = 0;
	memAlloc.memoryTypeIndex = 0;
	memAlloc.allocationSize = memRqrmnt.size;
	// Determine the type of memory required with the help of memory properties
	pass = vkDevice.memoryTypeFromProperties(memRqrmnt.memoryTypeBits, 0, /* No requirements */ &memAlloc.memoryTypeIndex);
	assert(pass);

	// Allocate the memory for image objects
	result = vkAllocateMemory(vkDevice.device, &memAlloc, NULL, &Depth.mem);
	assert(result == VK_SUCCESS);

	// Bind the allocated memeory
	result = vkBindImageMemory(vkDevice.device, Depth.image, Depth.mem, 0);
	assert(result == VK_SUCCESS);


	VkImageViewCreateInfo imgViewInfo = {};
	imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imgViewInfo.pNext = NULL;
	imgViewInfo.image = VK_NULL_HANDLE;
	imgViewInfo.format = depthFormat;
	imgViewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY };
	imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	imgViewInfo.subresourceRange.baseMipLevel = 0;
	imgViewInfo.subresourceRange.levelCount = 1;
	imgViewInfo.subresourceRange.baseArrayLayer = 0;
	imgViewInfo.subresourceRange.layerCount = 1;
	imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imgViewInfo.flags = 0;

	if (depthFormat == VK_FORMAT_D16_UNORM_S8_UINT ||
		depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
		depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) {
		imgViewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	// Use command buffer to create the depth image. This includes -
	// Command buffer allocation, recording with begin/end scope and submission.
	CommandBufferMgr::allocCommandBuffer(&vkDevice.device, cmdPool, &cmdDepthImage,nullptr,1);
	CommandBufferMgr::beginCommandBuffer(cmdDepthImage);
	{
		// Set the image layout to depth stencil optimal
		setImageLayout(Depth.image,
			imgViewInfo.subresourceRange.aspectMask,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, (VkAccessFlagBits)0, cmdDepthImage);
	}
	CommandBufferMgr::endCommandBuffer(cmdDepthImage);
	CommandBufferMgr::submitCommandBuffer(vkDevice.queue, &cmdDepthImage);

	// Create the image view and allow the application to use the images.
	imgViewInfo.image = Depth.image;
	result = vkCreateImageView(vkDevice.device, &imgViewInfo, NULL, &Depth.view);
	assert(result == VK_SUCCESS);

	Logger::log(0,"[Logger][Renderer] Depth Buffer created\n");

}

void Renderer::buildSwapChainAndDepthImage(){
    vkSwapchain.createColorImageView(cmdDepthImage,vkDevice);
}

void Renderer::destroyDepthBuffer(){
    vkDestroyImageView(vkDevice.device, Depth.view, NULL);
	vkDestroyImage(vkDevice.device, Depth.image, NULL);
	vkFreeMemory(vkDevice.device, Depth.mem, NULL);
	Logger::log(0,"[Logger][Renderer] Depth Buffer destroyed\n");



}



