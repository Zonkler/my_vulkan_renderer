
#include "tools/tools.hpp"
#include "engine/Renderer.hpp"
#include <vulkan/vulkan.h>
#include <iostream>
#include <cassert>

#define VMA_IMPLEMENTATION
#include "tools/exception.hpp"

#include <vma/vk_mem_alloc.h>
#include "engine/VulkanSwapchain.hpp"
#include "engine/Wrapper.hpp"
#include "engine/Shader.hpp"
#include "engine/VulkanGFXPipeline.hpp"
#include "engine/VulkanContext.hpp"
#include "tools/VulkanTools.hpp"
Renderer::Renderer(VulkanRenderData &rData, VulkanContext &vkContaxt, VkSwapchainKHR &swapchain, VulkanSwapchain &vkSwap) : renderData(rData), m_vkContext(vkContaxt), vkSwapchain(vkSwap)
{
    // Note: It's very important to initilize the member with 0 or respective value other wise it will break the system
    memset(&Depth, 0, sizeof(Depth));

    allocator = m_vkContext.allocator;

    createCommandPool();
    createDepthImage();
    buildSwapChainAndDepthImage();

    m_vkQueue.init(*m_vkContext.device, swapchain, m_vkContext.GraphicsQueueWithPresentationSupport, 0, m_vkContext.SwapchainImageCnt);
    
    ShaderModules.emplace_back(std::make_unique<Shader>(m_vkContext.device, VK_SHADER_STAGE_VERTEX_BIT, "skibidi", "../shaders/main.vert.spv"));
    ShaderModules.emplace_back(std::make_unique<Shader>(m_vkContext.device, VK_SHADER_STAGE_FRAGMENT_BIT, "skibidi2", "../shaders/main.frag.spv"));

    m_GFXPipeline.init(*m_vkContext.device, renderData, m_renderPass, ShaderModules, vkSwapchain.format, Depth.format);

    recordCommandbuffers();
}

Renderer::~Renderer()
{
    if (m_vkContext.device)
    {
        vkDeviceWaitIdle(*m_vkContext.device);
    }
    destroyDepthBuffer();

    m_vkQueue.destroy();
    destroyCommandBuffer();
    destroyCommandPool();
}

void Renderer::renderFrame()
{
    m_vkQueue.waitForCurrentFrameFence();

    uint32_t imageIndex = m_vkQueue.acquireNextImage();

    m_vkQueue.submitAsync(m_cmdBuffers[imageIndex], imageIndex);

    m_vkQueue.present(imageIndex);
}

void Renderer::createCommandPool()
{

    VkResult res;

    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.pNext = NULL;
    cmdPoolInfo.queueFamilyIndex = m_vkContext.graphicsQueueFamilyIndex;
    cmdPoolInfo.flags = 0;

    res = vkCreateCommandPool(*m_vkContext.device, &cmdPoolInfo, NULL, &cmdPool);
    Logger::log(0, "[Logger][Renderer] Command pool created\n");
    // assert(res == VK_SUCCESS);
}

void Renderer::destroyCommandPool()
{
    vkDestroyCommandPool(*m_vkContext.device, cmdPool, NULL);

    Logger::log(0, "[Logger][Renderer] Command pool destroyed\n");
}

void Renderer::destroyCommandBuffer()
{
    VkCommandBuffer cmdBufs[] = {cmdDepthImage};
    vkFreeCommandBuffers(*m_vkContext.device, cmdPool, sizeof(cmdBufs) / sizeof(VkCommandBuffer), cmdBufs);
}

void Renderer::recordCommandbuffers(){
    m_cmdBuffers.resize(m_vkContext.SwapchainImageCnt);

    CommandBufferMgr::allocCommandBuffer(m_vkContext.device, cmdPool, m_cmdBuffers.data(), nullptr, m_cmdBuffers.size());

    VkClearColorValue Clearcolor = {0.3f, 0.3f, 0.3f, 0.3f};
    VkClearValue clearValue;
    clearValue.color = Clearcolor;

    
    VkClearValue depthClearValue{};
    depthClearValue.depthStencil = {1.0f, 0}; // Depth=1.0, Stencil=0


        // For each command buffer:
    for (size_t i = 0; i < m_cmdBuffers.size(); i++)
    {
        CommandBufferMgr::beginCommandBuffer(m_cmdBuffers[i], nullptr);

        vkutils::setImageLayout(vkSwapchain.swapchainImages[i], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                VK_ACCESS_NONE,
                                m_cmdBuffers[i]);

        // Begin dynamic rendering
        VkRenderingAttachmentInfo colorAttachment{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = vkSwapchain.colorBuffer[i].view,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = clearValue
        };

        VkRenderingAttachmentInfo depthAttachment{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = Depth.view,
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .clearValue = depthClearValue
        };

        VkRenderingInfo renderInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = {{0, 0}, {renderData.rdWidth, renderData.rdHeight}},
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachment,
            .pDepthAttachment = &depthAttachment

        };
            
        vkCmdBeginRendering(m_cmdBuffers[i], &renderInfo);

        m_GFXPipeline.bind(m_cmdBuffers[i]);

        uint32_t vertexcount{3};
        uint32_t instancecount{1};
        uint32_t firstvertex{0};
        uint32_t firstinstance{0};

        vkCmdDraw(m_cmdBuffers[i], vertexcount, instancecount, firstvertex, firstinstance);

        // Drawing commands...
        vkCmdEndRendering(m_cmdBuffers[i]);

        vkutils::setImageLayout(
            vkSwapchain.swapchainImages[i],
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            m_cmdBuffers[i]);

        CommandBufferMgr::endCommandBuffer(m_cmdBuffers[i]);
    }


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
    vkGetPhysicalDeviceFormatProperties(*m_vkContext.pDevice, depthFormat, &props);
    if (!(props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
    {
        std::cerr << "Unsupported Depth Format for optimal tiling\n";
        exit(-1);
    }

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = depthFormat;
    imageInfo.extent.width = m_vkContext.swapchainExtent->width;
    imageInfo.extent.height = m_vkContext.swapchainExtent->height;
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
        *m_vkContext.allocator, // Your VMA allocator instance
        &imageInfo,
        &allocCreateInfo,
        &Depth.image,
        &Depth.allocation, // Note: You need to add this member: VmaAllocation Depth.allocation;
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

    result = vkCreateImageView(*m_vkContext.device, &imgViewInfo, nullptr, &Depth.view);
    assert(result == VK_SUCCESS);

    // Use a command buffer to transition the image layout to DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    CommandBufferMgr::allocCommandBuffer(m_vkContext.device, cmdPool, &cmdDepthImage, nullptr, 1);
    CommandBufferMgr::beginCommandBuffer(cmdDepthImage);
    {
        vkutils::setImageLayout(Depth.image,
                                imgViewInfo.subresourceRange.aspectMask,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                (VkAccessFlagBits)0,
                                cmdDepthImage);
    }
    CommandBufferMgr::endCommandBuffer(cmdDepthImage);
    CommandBufferMgr::submitCommandBuffer(m_vkContext.queue, &cmdDepthImage);

    Logger::log(0, "[Logger][Renderer] Depth Buffer created with VMA\n");
}

void Renderer::buildSwapChainAndDepthImage()
{
    vkSwapchain.createColorImageView(cmdDepthImage, *m_vkContext.device);
}

void Renderer::destroyDepthBuffer()
{
    // Destroy image view
    vkDestroyImageView(*m_vkContext.device, Depth.view, nullptr);

    // Destroy image and free memory using VMA
    vmaDestroyImage(*m_vkContext.allocator, Depth.image, Depth.allocation);

    Logger::log(0, "[Logger][Renderer] Depth Buffer destroyed (VMA)\n");
}


