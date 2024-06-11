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

struct RHIPipelineState
{
    VkVertexInputBindingDescription vertexInputBinding;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
    VkCullModeFlagBits cullingType;
    RHIDepthStencilState depthStencilState;
    std::vector<RHIBlendState> blendStates;
    std::vector<RHIShaderState> shaderStates;
    VkPipelineLayout layout;
};