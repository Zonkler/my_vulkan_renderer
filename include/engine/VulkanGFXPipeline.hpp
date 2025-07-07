#pragma once

#include <vulkan/vulkan.h>
#include "engine/VulkanRenderdata.hpp"
#include <memory>
#include "engine/Shader.hpp"
class VulkanGFXPipeline
{
private:

    VkDevice m_device = nullptr;
    VkPipeline m_pipeline = nullptr;
    VkPipelineLayout m_pipelineLayout = nullptr; 

public:
    VulkanGFXPipeline() = default;
    ~VulkanGFXPipeline()= default;

    void init(VkDevice& Device,VulkanRenderData& rdata,VkRenderPass RenderPass,std::vector<std::unique_ptr<Shader>>& ShaderModules);
    void destroy();
    void bind(VkCommandBuffer cmdbuff);
};


