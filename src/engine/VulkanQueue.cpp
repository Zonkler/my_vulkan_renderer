#include <vulkan/vulkan.h>

#include "engine/VulkanQueue.hpp"

#include "tools/tools.hpp"

#include <cassert>
void VulkanQueue::init(VkDevice& Device, VkSwapchainKHR swapchain, uint32_t queueFamily,uint32_t queueIndex){
    m_device    = &Device;
    m_swapChain = swapchain;

    vkGetDeviceQueue(Device,queueFamily,queueIndex,&m_queue);

    Logger::log(0,"[Logger][Vulkan Queue] Queue acquired \n");

    createSemaphores();
}

void VulkanQueue::createSemaphores(){

    

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

VkResult res1 = vkCreateSemaphore(*m_device, &createInfo, nullptr, &m_renderCompleteSem);
VkResult res2 = vkCreateSemaphore(*m_device, &createInfo, nullptr, &m_presentCompleteSem);

assert(res1 == VK_SUCCESS && "Failed to create renderCompleteSem");
assert(res2 == VK_SUCCESS && "Failed to create presentCompleteSem");

}

void VulkanQueue::waitIdle(){

    vkQueueWaitIdle(m_queue);

}

uint32_t VulkanQueue::acquireNextImage(){
if (m_swapChain == VK_NULL_HANDLE) {
	std::cerr << "[FATAL] Swapchain handle is NULL!\n";
	assert(false);
}

    uint32_t imageIndex = 0;
    VkResult res = vkAcquireNextImageKHR(*m_device,m_swapChain,UINT64_MAX,m_presentCompleteSem,nullptr,&imageIndex);
    if (res != VK_SUCCESS) {
	std::cerr << "[FATAL] vkAcquireNextImageKHR failed with error code: " << res << "\n";
	assert(false); // or return false from renderFrame
}
    return imageIndex;
}

void VulkanQueue::submitAsync(VkCommandBuffer& cmdBuf){

    VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_presentCompleteSem;
    submitInfo.pWaitDstStageMask = &waitFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuf;
    submitInfo.signalSemaphoreCount =1;
    submitInfo.pSignalSemaphores = &m_renderCompleteSem;

    VkResult res = vkQueueSubmit(m_queue,1,&submitInfo,nullptr);



}

void VulkanQueue::submitSync(VkCommandBuffer& cmdBuf){

    VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = VK_NULL_HANDLE;
    submitInfo.pWaitDstStageMask = VK_NULL_HANDLE;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuf;
    submitInfo.signalSemaphoreCount =0;
    submitInfo.pSignalSemaphores = VK_NULL_HANDLE;

    VkResult res = vkQueueSubmit(m_queue,1,&submitInfo,nullptr);



}

void VulkanQueue::present(uint32_t imageIndex){

    VkPresentInfoKHR PresentInfo{};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.pNext = nullptr;
    PresentInfo.waitSemaphoreCount= 1;
    PresentInfo.pWaitSemaphores = &m_renderCompleteSem;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &m_swapChain;
    PresentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(m_queue,&PresentInfo);


}
