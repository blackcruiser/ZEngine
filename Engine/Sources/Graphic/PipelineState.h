#pragma once

#include <string>

#include <vulkan/vulkan_core.h>


struct RHIDepthStencilState
{
    VkStencilOpState front, back;
    VkBool32 stencilTestEnable;
    VkBool32 depthTestEnable;
    VkBool32 depthWriteEnable;
    VkCompareOp depthCompareOp;
};

struct RHIBlendState
{
    VkBlendFactor srcFactor, dstFactor, srcAlphaFactor, dstAlphaFactor;
    VkBlendOp operation;
};

struct RHIShaderState
{
    VkShaderStageFlagBits stage;
    VkShaderModule shaderModule;
    std::string name;
};

struct RHIRasterizationState
{
    VkCullModeFlagBits cullingType;
};

struct RHIPipelineState
{
    RHIPipelineState() : vertexInputBinding{}, vertexInputState{}, rasterizeationState{}, inputAssemblyState{}, depthStencilState{}, colorBlendState{}, layout(VK_NULL_HANDLE)
    {
    }

    VkVertexInputBindingDescription vertexInputBinding;
    VkPipelineVertexInputStateCreateInfo vertexInputState;
    VkPipelineRasterizationStateCreateInfo rasterizeationState;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    VkPipelineColorBlendStateCreateInfo colorBlendState;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineLayout layout;
};