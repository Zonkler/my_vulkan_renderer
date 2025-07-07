#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class Shader
{
private:
    const VkDevice *m_device;
    VkShaderStageFlagBits m_type;
    std::string m_name;
    std::string m_entry_point;

    VkShaderModule m_shader_module{VK_NULL_HANDLE};

    static std::vector<uint32_t> readFile(const std::string& filename);


public:
    Shader(const VkDevice *device, VkShaderStageFlagBits type, const std::string &name, const std::string &file_name,
           const std::string &entry_point = "main");
    ~Shader();

    [[nodiscard]] const std::string &name() const
    {
        return m_name;
    }

    [[nodiscard]] const std::string &entry_point() const
    {
        return m_entry_point;
    }

    [[nodiscard]] VkShaderStageFlagBits type() const
    {
        return m_type;
    }

    [[nodiscard]] VkShaderModule module() const
    {
        return m_shader_module;
    }

};

