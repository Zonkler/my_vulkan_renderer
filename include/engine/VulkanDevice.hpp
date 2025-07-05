#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <vma/vk_mem_alloc.h>

class VulkanDevice
{
public:
    VulkanDevice()  = default;
    ~VulkanDevice() = default;

    int init(VkInstance& VKinst,std::vector<const char*> DeviceExtensions);
    void destroy();

	// Query physical device to retrive queue properties
	uint32_t getGraphicsQueueHandle();

    void getDeviceQueue();
    
    bool memoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t *typeIndex);

    VmaAllocator getAllocator() const { return allocator; }


    // Device related member variables
    VkDevice                         device;            // Logical device
    std::vector<VkPhysicalDevice>    physicalDevices;   // Vector of all available P. devices
    VkPhysicalDevice                 gpu;               // Physical device
    VkPhysicalDeviceProperties       gpuPros;           // Physical device properties
    VkPhysicalDeviceMemoryProperties memoryProperties;  


    // Queue related properties 
    VkQueue queue;                                          // Vulkan Queues object
    std::vector<VkQueueFamilyProperties> queueFamilyProps;  // Store all queue families exposed by the physical device.
    uint32_t graphicsQueueFamilyIndex;                      // Stores graphics queue index      
    uint32_t graphicsQueueWithPresentIndex;                 // Number of queue family exposed by device

    uint32_t queueFamilyCount;                              // Number of queue family exposed by device   

private:

    VmaAllocator allocator;

    void initializeVMA(VkInstance& VKinst);
    VkResult enumeratePhysicalDevices(VkInstance& VKinst); 
    void createDevice(std::vector<const char*> DeviceExtensions);

    void getPhysicalDeviceQueuesAndProperties(); // Get the avaialbe queues exposed by the physical devices
    
};



