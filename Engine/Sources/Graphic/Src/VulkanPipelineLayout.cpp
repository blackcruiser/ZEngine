#include "VulkanPipelineLayout.h"
#include "VulkanDescriptorSetLayout.h"
#include "Misc/AssertionMacros.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>


namespace ZE {

VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice* device, std::vector<VulkanDescriptorSetLayout*>& descriptorSetLayoutArr)
    : VulkanDeviceChild(device), _descriptorSetLayoutArr(descriptorSetLayoutArr), _pipelineLayout(VK_NULL_HANDLE)
{
    std::vector<VkDescriptorSetLayout> layoutArr{};
    std::transform(
        descriptorSetLayoutArr.begin(), descriptorSetLayoutArr.end(), std::back_inserter(layoutArr), [](VulkanDescriptorSetLayout* layout) {
            return layout->GetRawDescriptorSetLayout();
        });
 

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = layoutArr.size();
    pipelineLayoutInfo.pSetLayouts = layoutArr.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    VkResult result = vkCreatePipelineLayout(_device->GetRawDevice(), &pipelineLayoutInfo, nullptr, &_pipelineLayout);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create PipelineLayout!");
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
    CHECK(_pipelineLayout != VK_NULL_HANDLE);
    vkDestroyPipelineLayout(_device->GetRawDevice(), _pipelineLayout, nullptr);
}

VkPipelineLayout VulkanPipelineLayout::GetRawPipelineLayout()
{
    return _pipelineLayout;
}

} // namespace ZE