

#include <vulkan/vulkan.h>

#include <cassert>
#include <vector>
#include "tools/tools.hpp"

#include "engine/VulkanDevice.hpp"

#include "engine/VulkanContext.hpp"

namespace PyroCore
{
    
VulkanDevice::VulkanDevice(VulkanContext& vkContext ,std::vector<const char *> DeviceExtensions)
{

    VkInstance VKinst = *vkContext.instance;
    
    enumeratePhysicalDevices(VKinst);

    getPhysicalDeviceQueuesAndProperties();

    getGraphicsQueueHandle();

    createDevice(DeviceExtensions);
    getDeviceQueue();
    Logger::log(0, "[Logger][Device] Logical device created\n");
    initializeVMA(VKinst);

    vkContext.device = &device;
    vkContext.pDevice = &gpu;
    vkContext.allocator = &allocator;
    vkContext.graphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
    vkContext.queue = queue;
    

}

VulkanDevice::~VulkanDevice()
{

    vmaDestroyAllocator(allocator);
    Logger::log(0, "[Logger][Device] VMA destroyed\n");

    vkDestroyDevice(device, nullptr);
    Logger::log(0, "[Logger][Device] Logical device destroyed\n");
}

VkResult VulkanDevice::enumeratePhysicalDevices(VkInstance &VKinst)
{

    VkResult Result = VK_SUCCESS;

    // obtain total amount of physical devices available to the system
    uint32_t physicalDeviceCount{0};
    Result = vkEnumeratePhysicalDevices(VKinst, &physicalDeviceCount, nullptr);

    if (Result != VK_SUCCESS || physicalDeviceCount == 0)
    {
        Logger::log(1, "[ERROR][Logger][Device] No physical devices found or vkEnumerate failed\n");
        return Result;
    }

    physicalDevices.resize(physicalDeviceCount);

    Result = vkEnumeratePhysicalDevices(VKinst, &physicalDeviceCount, &physicalDevices[0]);
    assert(Result == VK_SUCCESS);

    // TODO: rank physical devices based on their properties and pick the bes one
    vkGetPhysicalDeviceProperties(physicalDevices[0],&gpuPros);

    // pick the first
    gpu = physicalDevices[0];

    Logger::log(0, "[Logger][Device] Physical device picked ({})\n",gpuPros.deviceName);

    return Result;
}

void VulkanDevice::getPhysicalDeviceQueuesAndProperties()
{

    // Query queue families count with pass NULL as second parameter.
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, NULL);

    // Allocate space to accomodate Queue properties.
    queueFamilyProps.resize(queueFamilyCount);

    // Get queue family properties
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilyProps.data());
}

void VulkanDevice::getGraphicsQueueHandle()
{

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

    if (found)
    {
        Logger::log(0, "[Logger][Device] Graphics queue located\n");

    }

    else
    {
        Logger::log(0, "[Logger][Device] Graphics queue could not belocated\n");
    }
    


}

void VulkanDevice::createDevice(std::vector<const char *> DeviceExtensions)
{
    DeviceExtensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);

    VkResult result = VK_SUCCESS;
    float queuePriority = 0.0;

    // Create the object information
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    queueInfo.pNext = NULL;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.dynamicRendering = VK_TRUE;

    // create the device information
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = &features13;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = NULL;
    deviceInfo.enabledExtensionCount = DeviceExtensions.size();
    deviceInfo.ppEnabledExtensionNames = DeviceExtensions.data();
    deviceInfo.pEnabledFeatures = NULL;

    // create the device itself
    result = vkCreateDevice(gpu, &deviceInfo, NULL, &device);
    assert(result == VK_SUCCESS);
}

void VulkanDevice::getDeviceQueue()
{
    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &queue);
};

bool VulkanDevice::memoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t *typeIndex)
{
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++)
    {
        if ((typeBits & 1) == 1)
        {
            // Type is available, does it match user properties?
            if ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
            {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

void VulkanDevice::initializeVMA(VkInstance &VKinst)
{

    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice = gpu;
    allocatorInfo.device = device;
    allocatorInfo.instance = VKinst;
    vmaCreateAllocator(&allocatorInfo, &allocator);
    Logger::log(0, "[Logger][Device] VMA initialized\n");
}

} // namespace PyroCore