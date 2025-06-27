#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

class VulkanDevice
{
public:
    VulkanDevice(VkInstance& VKinst,std::vector<const char*> DeviceExtensions);
    ~VulkanDevice();

    VkResult enumeratePhysicalDevices(VkInstance& VKinst); 
    VkResult createDevice(std::vector<const char*> DeviceExtensions);
    
	void getPhysicalDeviceQueuesAndProperties(); // Get the avaialbe queues exposed by the physical devices

	// Query physical device to retrive queue properties
	uint32_t getGraphicsQueueHandle();
    void getDeviceQueue();

    // Device related member variables
    VkDevice                         device;            // Logical device
    std::vector<VkPhysicalDevice>    physicalDevices;   // Vector of all available P. devices
    VkPhysicalDevice                 gpu;               // Physical device
    VkPhysicalDeviceProperties       gpuPros;           // Physical device properties
    VkPhysicalDeviceMemoryProperties memoryProperties;  


    // Queue related properties 
    VkQueue queue;  // Vulkan Queues object
    std::vector<VkQueueFamilyProperties> queueFamilyProps;  // Store all queue families exposed by the physical device.
    uint32_t graphicsQueueFamilyIndex;  // Stores graphics queue index      
    uint32_t queueFamilyCount;  // Number of queue family exposed by device   



    
};



