
#include "tools/tools.hpp"
#include "engine/Renderer.hpp"
#include <vulkan/vulkan.h>
#include <iostream>
#include <cassert>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>


#define VMA_IMPLEMENTATION
#include "tools/exception.hpp"
#include "engine/VulkanRenderdata.hpp"
#include <vma/vk_mem_alloc.h>
#include "engine/VulkanSwapchain.hpp"
#include "engine/Wrapper.hpp"
#include "engine/Shader.hpp"
#include "engine/VulkanGFXPipeline.hpp"
#include "engine/VulkanContext.hpp"
#include "tools/VulkanTools.hpp"
#include "engine/VulkanVertexBuffer.hpp"
#include "model/gameobject.hpp"
#include "tools/camera.hpp"

namespace PyroCore
{
    
Renderer::Renderer(VulkanRenderData &rData, VulkanContext &vkContaxt, std::shared_ptr<PyroCore::VulkanSwapchain> swapchain2) : renderData(rData), m_vkContext(vkContaxt), vkSwapchain(std::move(swapchain2))
{
    
    // Note: It's very important to initilize the member with 0 or respective value other wise it will break the system
    memset(&Depth, 0, sizeof(Depth));

    allocator = m_vkContext.allocator;
    swapchain = vkSwapchain->swapChain;
    createCommandPool();
    createDepthImage();
    buildSwapChainAndDepthImage();

    m_vkQueue = std::make_unique<VulkanQueue>(*m_vkContext.device, swapchain, m_vkContext.GraphicsQueueWithPresentationSupport, 0, m_vkContext.SwapchainImageCnt);

    ShaderModules.emplace_back(std::make_unique<Shader>(m_vkContext.device, VK_SHADER_STAGE_VERTEX_BIT, "skibidi", "../shaders/main.vert.spv"));
    ShaderModules.emplace_back(std::make_unique<Shader>(m_vkContext.device, VK_SHADER_STAGE_FRAGMENT_BIT, "skibidi2", "../shaders/main.frag.spv"));

    m_GFXPipeline = std::make_unique<VulkanGFXPipeline>(*m_vkContext.device, renderData, m_renderPass, ShaderModules, vkSwapchain->format, Depth.format);

    triangle = std::make_shared<Model>(*allocator);

    auto gameObj = gameobject::createGameObject();
    gameObj.model = triangle;
    gameObj.transform2D.translation = {1.5f,0.5f,5.5f};
    gameObj.transform2D.scale = {0.5f,0.5f,0.5f};
    gameObjects.emplace_back(gameObj);

    auto gameObj2 = gameobject::createGameObject();
    gameObj2.model = triangle;
    gameObj2.transform2D.translation = {1.5f,0.5f,2.5f};
    gameObj2.transform2D.scale = {0.5f,.5f,0.5f};
    gameObjects.emplace_back(gameObj2);

    m_cmdBuffers.resize(m_vkContext.SwapchainImageCnt);
    CommandBufferMgr::allocCommandBuffer(m_vkContext.device, cmdPool, &m_cmdBuffers[0], nullptr, m_cmdBuffers.size());
    m_camera.setViewDirection(glm::vec3(0.f),glm::vec3(0.5f,0.f,1.f));
    m_camera.setPerspectiveProjection(glm::radians(50.f),vkSwapchain->extentAspectRatio(),0.1f,10.f);

}

Renderer::~Renderer()
{
    
    if (m_vkContext.device)
    {
        vkDeviceWaitIdle(*m_vkContext.device);
    }
    destroyDepthBuffer();

    //m_vkQueue.destroy();
    destroyCommandBuffer();
    destroyCommandPool();
}

void Renderer::renderFrame()
{
    m_vkQueue->waitForCurrentFrameFence();

    uint32_t imageIndex = m_vkQueue->acquireNextImage();

    recordCommandbuffers(imageIndex);

    m_vkQueue->submitAsync(m_cmdBuffers[imageIndex], imageIndex);

    m_vkQueue->present(imageIndex);
}

void Renderer::createCommandPool()
{

    VkResult res;

    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.pNext = NULL;
    cmdPoolInfo.queueFamilyIndex = m_vkContext.graphicsQueueFamilyIndex;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    res = vkCreateCommandPool(*m_vkContext.device, &cmdPoolInfo, NULL, &cmdPool);
    Logger::log(0, "[Logger][Renderer] Command pool created\n");
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

void Renderer::recordCommandbuffers(uint32_t index){
  
    auto& cmd = m_cmdBuffers[index];
    vkResetCommandBuffer(m_cmdBuffers[index], 0); // required if re-recording
    VkClearColorValue Clearcolor = {0.1f, 0.1f, 0.1f, 0.1f};
    VkClearValue clearValue;
    clearValue.color = Clearcolor;

    VkClearValue depthClearValue{};
    depthClearValue.depthStencil = {1.0f, 0}; // Depth=1.0, Stencil=0




        CommandBufferMgr::beginCommandBuffer(cmd, nullptr);

        vkutils::setImageLayout(vkSwapchain->swapchainImages[index], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                VK_ACCESS_NONE,
                                cmd);

        // Begin dynamic rendering
        VkRenderingAttachmentInfo colorAttachment{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = vkSwapchain->colorBuffer[index].view,
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
            
        vkCmdBeginRendering(cmd, &renderInfo);

        m_GFXPipeline->bind(cmd);

        VkBuffer vertexBuffers[] = { triangle->m_mesh.m_vertexBuffer.m_buffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);

        auto projectionView = m_camera.getProjection() * m_camera.getView();

            for (auto& obj : gameObjects)
            {   
                obj.transform2D.rotation.y = glm::mod(obj.transform2D.rotation.y +0.0001f,glm::two_pi<float>());
                obj.transform2D.rotation.x = glm::mod(obj.transform2D.rotation.y +0.00005f,glm::two_pi<float>());

                VkPushConstants push{};
                //push.offset= obj.transform2D.translation;
                push.color = obj.color;
                push.transform = projectionView * obj.transform2D.mat4();
                vkCmdPushConstants(cmd,m_GFXPipeline->m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
                ,0,sizeof(VkPushConstants),&push);


                vkCmdDraw(cmd, static_cast<uint32_t>(triangle->m_mesh.m_vertices.size()), 1, 0, 0);

            }


        vkCmdEndRendering(cmd);

        vkutils::setImageLayout(
            vkSwapchain->swapchainImages[index],
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            cmd);

        CommandBufferMgr::endCommandBuffer(cmd);



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
    vkSwapchain->createColorImageView(cmdDepthImage, *m_vkContext.device);
}

void Renderer::destroyDepthBuffer()
{
    // Destroy image view
    vkDestroyImageView(*m_vkContext.device, Depth.view, nullptr);

    // Destroy image and free memory using VMA
    vmaDestroyImage(*m_vkContext.allocator, Depth.image, Depth.allocation);

    Logger::log(0, "[Logger][Renderer] Depth Buffer destroyed (VMA)\n");
}

void Renderer::recreateSwapchain(std::shared_ptr<PyroCore::VulkanSwapchain>& swapchain2){

    vkDeviceWaitIdle(*m_vkContext.device);
    
    
    destroyDepthBuffer();
    vkSwapchain = swapchain2;
    swapchain = vkSwapchain->swapChain;
    createDepthImage();
    buildSwapChainAndDepthImage();
    m_vkQueue = std::make_unique<VulkanQueue>(*m_vkContext.device, swapchain, m_vkContext.GraphicsQueueWithPresentationSupport, 0, m_vkContext.SwapchainImageCnt);
    m_GFXPipeline = std::make_unique<VulkanGFXPipeline>(*m_vkContext.device, renderData, m_renderPass, ShaderModules, vkSwapchain->format, Depth.format);


}

} // namespace PyroCore