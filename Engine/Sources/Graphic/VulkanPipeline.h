#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;
class VulkanShader;
class VulkanPipelineLayout;
class VulkanRenderPass;

class VulkanGraphicPipeline
{
public:
    VulkanGraphicPipeline(TPtr<VulkanDevice> device, TPtr<VulkanShader> vertexShader, TPtr<VulkanShader> fragmentShader,
                          const VkExtent2D& extent, VkVertexInputBindingDescription& bindingDescription,
                          std::vector<VkVertexInputAttributeDescription>& attributeDescriptions, TPtr<VulkanPipelineLayout> pipelineLayout, TPtr<VulkanRenderPass> renderPass);
    ~VulkanGraphicPipeline();

    VkPipeline GetRawPipeline();

private:
    VkPipeline _vkPipeline;

    TPtr<VulkanDevice> _device;
};

} // namespace ZE