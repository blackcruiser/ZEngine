#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "PipelineState.h"
#include "VulkanDevice.h"
#include "GraphicResource.h"


namespace ZE {

class VulkanGraphicPipeline : public VulkanDeviceChild, public GraphicResource
{
public:
    VulkanGraphicPipeline(VulkanDevice* device, const VkGraphicsPipelineCreateInfo& createInfo);
    ~VulkanGraphicPipeline();

    VkPipeline GetRawPipeline();

private:
    VkPipeline _pipeline;
};

} // namespace ZE