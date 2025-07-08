#include "engine/VulkanGFXPipeline.hpp"
#include "tools/tools.hpp"


void VulkanGFXPipeline::init(VkDevice& Device,VulkanRenderData& rdata,VkRenderPass RenderPass,std::vector<std::unique_ptr<Shader>>& ShaderModules){

    m_device = Device;

    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2]{};


    shaderStageCreateInfo[0].sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[0].stage=VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfo[0].module= ShaderModules[0]->module();
    shaderStageCreateInfo[0].pName = "main";

    shaderStageCreateInfo[1].sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[1].stage=VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfo[1].module= ShaderModules[1]->module();
    shaderStageCreateInfo[1].pName = "main";

    VkPipelineVertexInputStateCreateInfo VertexInputInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
    };

    VkPipelineInputAssemblyStateCreateInfo PipelineIACreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE

    };

    VkViewport VP{
        .x = 0.0f,
        .y = 0.0f,
        .width= static_cast<float>(rdata.rdWidth),
        .height=static_cast<float>(rdata.rdHeight),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D Scissor{
        .offset{
            .x=0,
            .y=0
        },

        .extent{
            .width=static_cast<uint32_t>(rdata.rdWidth),
            .height=static_cast<uint32_t>(rdata.rdHeight)
            
        }

    };

    VkPipelineViewportStateCreateInfo VPCreateinfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &VP,
        .scissorCount = 1,
        .pScissors = &Scissor
    };

    VkPipelineRasterizationStateCreateInfo RastCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .lineWidth =1.0f

    };

    VkPipelineMultisampleStateCreateInfo PipelineMSCrateinfo{
        .sType= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f
    };

    VkPipelineColorBlendAttachmentState BlendAttachState{
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,

    };

    VkPipelineColorBlendStateCreateInfo BlendCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &BlendAttachState
    };

    VkPipelineLayoutCreateInfo LayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr
    };

    vkCreatePipelineLayout(m_device,&LayoutInfo,nullptr,&m_pipelineLayout);

    VkGraphicsPipelineCreateInfo PipelineInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages= &shaderStageCreateInfo[0],
        .pVertexInputState = &VertexInputInfo,
        .pInputAssemblyState= &PipelineIACreateInfo,
        .pViewportState = &VPCreateinfo,
        .pRasterizationState = &RastCreateInfo,
        .pMultisampleState = &PipelineMSCrateinfo,
        .pColorBlendState = &BlendCreateInfo,
        .layout = m_pipelineLayout,
        .renderPass= RenderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    vkCreateGraphicsPipelines(m_device,VK_NULL_HANDLE,1,&PipelineInfo,nullptr,&m_pipeline);
    Logger::log(0,"[Logger][Graphics Pipeline] Pipeline created\n");

}

void VulkanGFXPipeline::bind(VkCommandBuffer cmdbuff){
    vkCmdBindPipeline(cmdbuff,VK_PIPELINE_BIND_POINT_GRAPHICS,m_pipeline);
}

VulkanGFXPipeline::~VulkanGFXPipeline(){

    vkDestroyPipeline(m_device,m_pipeline,nullptr);
    Logger::log(0,"[Logger][Graphics Pipeline] Pipeline destroyed\n");
    vkDestroyPipelineLayout(m_device,m_pipelineLayout,nullptr);
    Logger::log(0,"[Logger][Graphics Pipeline] Pipeline Layout destroyed\n");

}