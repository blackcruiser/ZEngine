#include "VulkanPipelineLayout.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDevice.h"

#include <stdexcept>


namespace TE {

VulkanPipelineLayout::VulkanPipelineLayout(
    TPtr<VulkanDevice> device, TPtr<VulkanDescriptorSet> descriptorSet)
    : _device(device),  _vkPipelineLayout(VK_NULL_HANDLE)
{
    // PipelineLayout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSet->GetRawDescriptorSetLayout();
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(_device->GetRawDevice(), &pipelineLayoutInfo, nullptr, &_vkPipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
    vkDestroyPipelineLayout(_device->GetRawDevice(), _vkPipelineLayout, nullptr);
}

VkPipelineLayout VulkanPipelineLayout::GetRawPipelineLayout()
{
    return _vkPipelineLayout;
}

} // namespace TE