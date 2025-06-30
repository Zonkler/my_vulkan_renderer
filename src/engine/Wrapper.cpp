#include <vulkan/vulkan.h>
#include "engine/Wrapper.hpp"
#include <cassert>
void CommandBufferMgr::allocCommandBuffer(const VkDevice* device, const VkCommandPool cmdPool, VkCommandBuffer* cmdBuf, const VkCommandBufferAllocateInfo* commandBufferInfo)
{
	// Dependency on the intialize SwapChain Extensions and initialize CommandPool
	VkResult result;

	// If command information is available use it as it is.
	if (commandBufferInfo) {
		result = vkAllocateCommandBuffers(*device, commandBufferInfo, cmdBuf);
		assert(!result);
		return;
	}

	// Default implementation, create the command buffer
	// allocation info and use the supplied parameter into it
	VkCommandBufferAllocateInfo cmdInfo = {};
	cmdInfo.sType		= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdInfo.pNext		= NULL;
	cmdInfo.commandPool = cmdPool;
	cmdInfo.level		= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdInfo.commandBufferCount = (uint32_t) sizeof(cmdBuf) / sizeof(VkCommandBuffer);;

	result = vkAllocateCommandBuffers(*device, &cmdInfo, cmdBuf);
	assert(!result);
}

void CommandBufferMgr::beginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferBeginInfo* inCmdBufInfo)
{
	// Dependency on  the initialieCommandBuffer()
	VkResult  result;
	// If the user has specified the custom command buffer use it
	if (inCmdBufInfo) {
		result = vkBeginCommandBuffer(cmdBuf, inCmdBufInfo);
		assert(result == VK_SUCCESS);
		return;
	}

	// Otherwise, use the default implementation.
	VkCommandBufferInheritanceInfo cmdBufInheritInfo = {};
	cmdBufInheritInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	cmdBufInheritInfo.pNext					= NULL;
	cmdBufInheritInfo.renderPass			= VK_NULL_HANDLE;
	cmdBufInheritInfo.subpass				= 0;
	cmdBufInheritInfo.framebuffer			= VK_NULL_HANDLE;
	cmdBufInheritInfo.occlusionQueryEnable	= VK_FALSE;
	cmdBufInheritInfo.queryFlags			= 0;
	cmdBufInheritInfo.pipelineStatistics	= 0;
	
	VkCommandBufferBeginInfo cmdBufInfo = {};
	cmdBufInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufInfo.pNext				= NULL;
	cmdBufInfo.flags				= 0;
	cmdBufInfo.pInheritanceInfo		= &cmdBufInheritInfo;

	result = vkBeginCommandBuffer(cmdBuf, &cmdBufInfo);

	assert(result == VK_SUCCESS);
}

void CommandBufferMgr::endCommandBuffer(VkCommandBuffer commandBuffer)
{
	VkResult  result;
	result = vkEndCommandBuffer(commandBuffer);
	assert(result == VK_SUCCESS);
}

void CommandBufferMgr::submitCommandBuffer(const VkQueue& queue, const VkCommandBuffer* commandBuffer, const VkSubmitInfo* inSubmitInfo, const VkFence& fence)
{
	VkResult result;
	
	// If Subimt information is avialable use it as it is, this assumes that 
	// the commands are already specified in the structure, hence ignore command buffer 
	if (inSubmitInfo){
		result = vkQueueSubmit(queue, 1, inSubmitInfo, fence);
		assert(!result);

		result = vkQueueWaitIdle(queue);
		assert(!result);
		return;
	}

	// Otherwise, create the submit information with specified buffer commands
	VkSubmitInfo submitInfo = {};
	submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext				= NULL;
	submitInfo.waitSemaphoreCount	= 0;
	submitInfo.pWaitSemaphores		= NULL;
	submitInfo.pWaitDstStageMask	= NULL;
	submitInfo.commandBufferCount	= (uint32_t)sizeof(commandBuffer)/sizeof(VkCommandBuffer);
	submitInfo.pCommandBuffers		= commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores	= NULL;

	result = vkQueueSubmit(queue, 1, &submitInfo, fence);
	assert(!result);

	result = vkQueueWaitIdle(queue);
	assert(!result);

	// Double check this following code need to be implemented.
	//do {
	//	res = vkWaitForFences(appObj->deviceObj->device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
	//} while (res == VK_TIMEOUT);
	//assert(res == VK_SUCCESS);

	//vkDestroyFence(appObj->deviceObj->device, drawFence, NULL);
}