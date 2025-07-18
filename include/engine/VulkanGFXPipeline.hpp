#pragma once

#include <vulkan/vulkan.h>
#include "engine/VulkanRenderdata.hpp"
#include <memory>
#include "engine/Shader.hpp"
class VulkanGFXPipeline
{
public:

    VkDevice m_device = nullptr;
    VkPipeline m_pipeline = nullptr;
    VkPipelineLayout m_pipelineLayout = nullptr; 
    VkDescriptorSetLayout m_descriptorSetLayout;

public:
    VulkanGFXPipeline(VkDevice& Device,VulkanRenderData& rdata,VkRenderPass RenderPass,std::vector<std::unique_ptr<Shader>>& ShaderModules,VkFormat colorFormat,VkFormat depthFormat);

    VulkanGFXPipeline(const VulkanGFXPipeline&)           = delete;
    VulkanGFXPipeline(VulkanGFXPipeline&&)                = delete;
    VulkanGFXPipeline operator=(const VulkanGFXPipeline&) = delete;
    VulkanGFXPipeline operator=(VulkanGFXPipeline&&)      = delete;
    
    ~VulkanGFXPipeline();

   // void init(VkDevice& Device,VulkanRenderData& rdata,VkRenderPass RenderPass,std::vector<std::unique_ptr<Shader>>& ShaderModules,VkFormat colorFormat,VkFormat depthFormat);
    //void destroy();
    void bind(VkCommandBuffer cmdbuff);
};


