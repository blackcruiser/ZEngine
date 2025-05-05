#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "PipelineState.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanRenderPass;

class VulkanGraphicPipeline : public VulkanDeviceChild
{
public:
    VulkanGraphicPipeline(VulkanDevice* device, const RHIPipelineState& state, VulkanRenderPass* renderPass);
    ~VulkanGraphicPipeline();

    VkPipeline GetRawPipeline();

private:
    VkPipeline _pipeline;

    VulkanRenderPass* _renderPass;
};

} // namespace ZE