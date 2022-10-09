#include "VulkanPipelineLayout.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDevice.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>


namespace ZE {

VulkanPipelineLayout::VulkanPipelineLayout(TPtr<VulkanDevice> device, TPtrArr<VulkanDescriptorSetLayout>& descriptorSetLayoutArr)
    : _device(device), _descriptorSetLayoutArr(descriptorSetLayoutArr), _vkPipelineLayout(VK_NULL_HANDLE)
{
    std::vector<VkDescriptorSetLayout> layoutArr{};
    std::transform(
        descriptorSetLayoutArr.begin(), descriptorSetLayoutArr.end(), std::back_inserter(layoutArr), [](TPtr<VulkanDescriptorSetLayout>& layout) {
            return layout->GetRawDescriptorSetLayout();
        });
 

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = layoutArr.size();
    pipelineLayoutInfo.pSetLayouts = layoutArr.data();
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

} // namespace ZE