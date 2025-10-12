#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanPipelineLayout.h"
#include "Debug/AssertionMacros.h"

#include <array>


namespace ZE {

VulkanGraphicPipeline::VulkanGraphicPipeline(
    VulkanDevice* device, const VkGraphicsPipelineCreateInfo& createInfo)
    : VulkanDeviceChild(device), _pipeline(VK_NULL_HANDLE)
{
    VkResult result = vkCreateGraphicsPipelines(_device->GetRawDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &_pipeline);
    ZE_CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create GraphicsPipeline!")
}

VulkanGraphicPipeline::~VulkanGraphicPipeline()
{
    ZE_CHECK(_pipeline != VK_NULL_HANDLE);
    vkDestroyPipeline(_device->GetRawDevice(), _pipeline, nullptr);
}

VkPipeline VulkanGraphicPipeline::GetRawPipeline()
{
    return _pipeline;
}

} // namespace ZE