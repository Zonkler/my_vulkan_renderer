#pragma once
#include <vulkan/vulkan.h>

#include <stdexcept>
#include <string>



class MagmaException : public std::runtime_error {
public:

    using std::runtime_error::runtime_error;
};


class VulkanException final : public MagmaException {
public:

    VulkanException(std::string message, VkResult result);
};

inline void vk_check(VkResult result, const std::string& message = "Vulkan error") {
    if (result != VK_SUCCESS) {
        throw VulkanException(message, result);
    }
}