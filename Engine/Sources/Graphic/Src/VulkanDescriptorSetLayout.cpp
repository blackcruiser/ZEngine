#include "VulkanDescriptorSetLayout.h"
#include "VulkanDevice.h"

#include <stdexcept>


namespace ZE {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(TPtr<VulkanDevice> device, const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings)
    : _device(device), _vkDescriptorSetLayout(VK_NULL_HANDLE)
{
    VkDevice vkDevice = _device->GetRawDevice();

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, nullptr, &_vkDescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
    VkDevice vkDevice = _device->GetRawDevice();

    vkDestroyDescriptorSetLayout(vkDevice, _vkDescriptorSetLayout, nullptr);
}

VkDescriptorSetLayout VulkanDescriptorSetLayout::GetRawDescriptorSetLayout()
{
    return _vkDescriptorSetLayout;
}
} // namespace ZE