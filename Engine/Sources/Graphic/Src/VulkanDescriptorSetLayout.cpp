#include "VulkanDescriptorSetLayout.h"
#include "Misc/AssertionMacros.h"


namespace ZE {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice* device, const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings)
    : VulkanDeviceChild(device), _descriptorSetLayout(VK_NULL_HANDLE)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings = layoutBindings.data();

    VkResult result = vkCreateDescriptorSetLayout(_device->GetRawDevice(), &layoutInfo, nullptr, &_descriptorSetLayout);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "failed to create DescriptorSetLayout!");
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(_device->GetRawDevice(), _descriptorSetLayout, nullptr);
}

VkDescriptorSetLayout VulkanDescriptorSetLayout::GetRawDescriptorSetLayout()
{
    return _descriptorSetLayout;
}

} // namespace ZE