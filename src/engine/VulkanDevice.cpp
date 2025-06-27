#include "engine/VulkanDevice.hpp"
#include <vulkan/vulkan.h>
#include "tools/tools.hpp"
#include <cassert>
VkResult VulkanDevice::enumeratePhysicalDevices(VkInstance& VKinst){

    VkResult Result = VK_SUCCESS;

    //obtain total amount of physical devices available to the system
    uint32_t physicalDeviceCount{0};
    Result = vkEnumeratePhysicalDevices(VKinst,&physicalDeviceCount,nullptr);

    if (Result != VK_SUCCESS || physicalDeviceCount == 0) {
        Logger::log(1, "ERROR::DEVICE:: No physical devices found or vkEnumerate failed\n");
        return Result;
    }

    physicalDevices.resize(physicalDeviceCount);
    Result = vkEnumeratePhysicalDevices(VKinst,&physicalDeviceCount,&physicalDevices[0]);
    assert(Result == VK_SUCCESS);

    // TODO: rank physical devices based on their properties and pick the bes one
    //VkPhysicalDeviceProperties physicalDeviceProp;
    //vkGetPhysicalDeviceProperties(physicalDevices[0],&physicalDeviceProp);

    //pick the first
    gpu = physicalDevices[0];



    Logger::log(0,"LOGGER::DEVICE:: Physical device picked\n");

    return Result;
}

VulkanDevice::VulkanDevice(VkInstance& VKinst,std::vector<const char*> DeviceExtensions)
{
    enumeratePhysicalDevices(VKinst);
    getPhysicalDeviceQueuesAndProperties();
    getGraphicsQueueHandle();
    createDevice(DeviceExtensions);
    getDeviceQueue();
    Logger::log(0,"LOGGER::DEVICE:: Logical device created\n");
    
}

VulkanDevice::~VulkanDevice()
{
    Logger::log(0,"LOGGER::DEVICE:: Logical device destroyed\n");
    vkDestroyDevice(device,nullptr);
}

VkResult VulkanDevice::createDevice(std::vector<const char*> DeviceExtensions){

    VkResult result = VK_SUCCESS;
    float queuePriority = 0.0;

    // Create the object information
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    queueInfo.pNext = NULL;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;

    //create the device information
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = NULL;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = NULL;
    deviceInfo.enabledExtensionCount = DeviceExtensions.size();
    deviceInfo.ppEnabledExtensionNames = DeviceExtensions.data();
    deviceInfo.pEnabledFeatures = NULL;
    
    //create the device itself
    result = vkCreateDevice(gpu, &deviceInfo, NULL, &device);
    
    assert(result == VK_SUCCESS);

    

    return result;



}

void VulkanDevice::getPhysicalDeviceQueuesAndProperties(){

    // Query queue families count with pass NULL as second parameter.
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, NULL);
	
	// Allocate space to accomodate Queue properties.
	queueFamilyProps.resize(queueFamilyCount);

	// Get queue family properties
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilyProps.data());


}


uint32_t VulkanDevice::getGraphicsQueueHandle(){

    bool found = false;

    for (size_t i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            found = true;
            graphicsQueueFamilyIndex = i;
            break;

        }
        
    }
    Logger::log(0,"LOGGER::DEVICE:: Graphics queue located\n");
    return 0;
    



}

void VulkanDevice::getDeviceQueue(){
    vkGetDeviceQueue(device,graphicsQueueFamilyIndex,0,&queue);



};