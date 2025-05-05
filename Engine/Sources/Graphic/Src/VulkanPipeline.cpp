#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanPipelineLayout.h"
#include "VulkanRenderPass.h"
#include "Misc/AssertionMacros.h"

#include <array>


namespace ZE {

VulkanGraphicPipeline::VulkanGraphicPipeline(
    VulkanDevice* device, const RHIPipelineState& state, VulkanRenderPass* renderPass)
    : VulkanDeviceChild(device),  _renderPass(renderPass), _pipeline(VK_NULL_HANDLE)
{
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = nullptr;
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;          // Optional
    multisampling.pSampleMask = nullptr;            // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE;      // Optional

    // Set the state as being dynamic
    std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{}; 
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(state.shaderStages.size());
    pipelineInfo.pStages = state.shaderStages.data();
    pipelineInfo.pVertexInputState = &state.vertexInputState;
    pipelineInfo.pInputAssemblyState = &state.inputAssemblyState;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &state.rasterizeationState;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &state.depthStencilState; // Optional
    pipelineInfo.pColorBlendState = &state.colorBlendState;
    pipelineInfo.pDynamicState = &dynamicStateCreateInfo; // Optional
    pipelineInfo.layout = state.layout;
    pipelineInfo.renderPass = renderPass->GetRawRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    VkResult result = vkCreateGraphicsPipelines(_device->GetRawDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create GraphicsPipeline!")
}

VulkanGraphicPipeline::~VulkanGraphicPipeline()
{
    CHECK(_pipeline != VK_NULL_HANDLE);
    vkDestroyPipeline(_device->GetRawDevice(), _pipeline, nullptr);
}

VkPipeline VulkanGraphicPipeline::GetRawPipeline()
{
    return _pipeline;
}

} // namespace ZE