
#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>
#include "engine/Shader.hpp"
#include "tools/tools.hpp"
#include <filesystem>


Shader::Shader(const VkDevice *device, VkShaderStageFlagBits type, const std::string &name, const std::string &file_name,
               const std::string &entry_point) : m_device(device), m_type(type), m_name(name)
{
    auto code = readFile(file_name);

    VkShaderModuleCreateInfo createInfo{
        .sType =VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = createInfo.codeSize = code.size() * sizeof(uint32_t),
        .pCode = reinterpret_cast<const uint32_t*>(code.data())
    };

    if (vkCreateShaderModule(*m_device, &createInfo, nullptr, &m_shader_module) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module: " + file_name);
    }

    Logger::log(0,"[Logger][Shader] Shader created with size {}b\n",code.size());
}

Shader::~Shader() {
    if (m_shader_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(*m_device, m_shader_module, nullptr);
    }
    Logger::log(0,"[Logger][Shader] Shader destroyed\n");

}

std::vector<uint32_t> Shader::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open shader file: " + filename);

    size_t fileSize = static_cast<size_t>(file.tellg());
    if (fileSize % 4 != 0)
        throw std::runtime_error("Shader file size is not a multiple of 4: " + filename);

    std::vector<uint32_t> buffer(fileSize / 4);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();

    return buffer;
}
