#include <vulkan/vulkan.h>

#include "engine/VulkanQueue.hpp"

#include "tools/tools.hpp"

#include <cassert>

VulkanQueue::VulkanQueue(VkDevice& Device, VkSwapchainKHR swapchain, uint32_t queueFamily,uint32_t queueIndex,uint32_t swapchainImageCount){
    m_device    = &Device;
    m_swapChain = swapchain;

    vkGetDeviceQueue(Device, queueFamily, queueIndex, &m_queue);
    Logger::log(0,"[Logger][Vulkan Queue] Queue acquired \n");

    // Reinitialize swapchain-size-dependent arrays
    imagesInFlight.clear();
    renderFinishedSemaphores.clear();
    imageSemaphoreOwner.clear();

    imagesInFlight.resize(swapchainImageCount, VK_NULL_HANDLE);
    renderFinishedSemaphores.resize(swapchainImageCount);
    imageSemaphoreOwner.resize(swapchainImageCount, SIZE_MAX);

    for (size_t i = 0; i < swapchainImageCount; i++) {
        VkSemaphoreCreateInfo semInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        vkCreateSemaphore(*m_device, &semInfo, nullptr, &renderFinishedSemaphores[i]);
    }

    createSemaphores();
    currentFrame = 0;
}

VulkanQueue::~VulkanQueue(){
    this->destroy();
}

void VulkanQueue::createSemaphores(){
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkSemaphoreCreateInfo semInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        vkCreateSemaphore(*m_device, &semInfo, nullptr, &imageAvailableSemaphores[i]);

        VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start signaled so first frame doesn't block
        vkCreateFence(*m_device, &fenceInfo, nullptr, &inFlightFences[i]);
    }
    Logger::log(0,"[Logger][Vulkan Queue] Semaphores & fences created \n");


}

void VulkanQueue::waitIdle(){

    vkQueueWaitIdle(m_queue);

}

uint32_t VulkanQueue::acquireNextImage() {


    uint32_t imageIndex = 0;
    VkResult res = vkAcquireNextImageKHR(*m_device, m_swapChain, UINT64_MAX,
                                         imageAvailableSemaphores[currentFrame],
                                         VK_NULL_HANDLE, &imageIndex);




    
    // Wait for the fence of the image (if in use)
    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(*m_device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    //Wait for the semaphore used by the previous frame that rendered to this image
    if (imageSemaphoreOwner[imageIndex] != SIZE_MAX &&
        imageSemaphoreOwner[imageIndex] != currentFrame) {
        // This semaphore was used in present and hasn't been reacquired since
        // So wait on its corresponding fence
        vkWaitForFences(*m_device, 1, &inFlightFences[imageSemaphoreOwner[imageIndex]], VK_TRUE, UINT64_MAX);
    }

    //Track usage
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];
    imageSemaphoreOwner[imageIndex] = currentFrame;

    return imageIndex;
}


void VulkanQueue::submitAsync(VkCommandBuffer& cmdBuf,uint32_t imageIndex) {
    VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
    submitInfo.pWaitDstStageMask = &waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuf;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphores[imageIndex];

    vkQueueSubmit(m_queue, 1, &submitInfo, inFlightFences[currentFrame]);
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

void VulkanQueue::present(uint32_t imageIndex) {
    VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores[imageIndex];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapChain;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(m_queue, &presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanQueue::waitForCurrentFrameFence() {
    vkWaitForFences(*m_device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(*m_device, 1, &inFlightFences[currentFrame]);
}

void VulkanQueue::destroy() {
    // Wait for the queue to finish using any resources
    if (m_queue) {
        vkQueueWaitIdle(m_queue);
    }

        // Reset all in-flight fences
    for (auto& fence : inFlightFences) {
        if (fence) vkResetFences(*m_device, 1, &fence);
    }

    // Destroy per-frame semaphores and fences
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (imageAvailableSemaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(*m_device, imageAvailableSemaphores[i], nullptr);
            imageAvailableSemaphores[i] = VK_NULL_HANDLE;
        }

        if (inFlightFences[i] != VK_NULL_HANDLE) {
            vkDestroyFence(*m_device, inFlightFences[i], nullptr);
            inFlightFences[i] = VK_NULL_HANDLE;
        }
    }

    // Destroy per-swapchain-image semaphores
    for (auto& semaphore : renderFinishedSemaphores) {
        if (semaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(*m_device, semaphore, nullptr);
            semaphore = VK_NULL_HANDLE;
        }
    }
    
    Logger::log(0,"[Logger][Vulkan Queue] Semaphores & fences destroyed \n");
}