#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>


namespace TE {

class VulkanDevice;
class VulkanShader;
class VulkanPipelineLayout;

class VulkanGraphicPipeline
{
public:
    VulkanGraphicPipeline(TPtr<VulkanDevice> device, TPtr<VulkanShader> vertexShader, TPtr<VulkanShader> fragmentShader,
                          const VkExtent2D& extent, VkVertexInputBindingDescription& bindingDescription,
                          std::vector<VkVertexInputAttributeDescription>& attributeDescriptions, TPtr<VulkanPipelineLayout> pipelineLayout, VkRenderPass renderPass);
    ~VulkanGraphicPipeline();

    VkPipeline GetRawPipeline();

private:
    VkPipeline _vkPipeline;

    TPtr<VulkanDevice> _device;
};

} // namespace TE