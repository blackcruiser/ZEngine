#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "PipelineState.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;
class VulkanShader;
class VulkanPipelineLayout;
class VulkanRenderPass;

class VulkanGraphicPipeline
{
public:
    VulkanGraphicPipeline(TPtr<VulkanDevice> device, const RHIPipelineState& state, TPtr<VulkanRenderPass> renderPass);
    ~VulkanGraphicPipeline();

    VkPipeline GetRawPipeline();

private:
    VkPipeline _vkPipeline;

    TPtr<VulkanDevice> _device;
    TPtr<VulkanRenderPass> _renderPass;
};

} // namespace ZE