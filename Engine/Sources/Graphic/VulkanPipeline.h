#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;
class VulkanShader;
class VulkanPipelineLayout;
class VulkanRenderPass;


struct VulkanGraphicPipelineDesc
{
    TPtr<VulkanShader> vertexShader;
    TPtr<VulkanShader> fragmentShader;
    VkExtent2D extent;
    VkVertexInputBindingDescription bindingDescription;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    TPtr<VulkanPipelineLayout> pipelineLayout;
};

class VulkanGraphicPipeline
{
public:
    VulkanGraphicPipeline(TPtr<VulkanDevice> device, const VulkanGraphicPipelineDesc& desc, TPtr<VulkanRenderPass> renderPass);
    ~VulkanGraphicPipeline();

    VkPipeline GetRawPipeline();

private:
    VkPipeline _vkPipeline;

    TPtr<VulkanDevice> _device;
    TPtr<VulkanShader> _vertexShader, _fragmentShader;
    TPtr<VulkanPipelineLayout> _pipelineLayout;
    TPtr<VulkanRenderPass> _renderPass;
};

} // namespace ZE