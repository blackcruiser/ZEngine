#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorPool.h"
#include "Misc/AssertionMacros.h"

#include <algorithm>
#include <iterator>


namespace ZE {

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice* device, VulkanDescriptorPool* descriptorPool, VulkanDescriptorSetLayout* descriptorSetLayout)
    : VulkanDeviceChild(device), _descriptorPool(descriptorPool), _descriptorSetLayout(descriptorSetLayout), _vkDescriptorSet(VK_NULL_HANDLE)
{
    VkDescriptorSetLayout vkDescriptorSetLayout = descriptorSetLayout->GetRawDescriptorSetLayout();

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool->GetRawDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &vkDescriptorSetLayout;

    VkResult result = vkAllocateDescriptorSets(_device->GetRawDevice(), &allocInfo, &_vkDescriptorSet);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "failed to allocate DescriptorSets!");
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    vkFreeDescriptorSets(_device->GetRawDevice(), _descriptorPool->GetRawDescriptorPool(), 1, &_vkDescriptorSet);
}

void VulkanDescriptorSet::Update(uint32_t binding, uint32_t arrayElement, const VkDescriptorBufferInfo& bufferInfo)
{
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = _vkDescriptorSet;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = arrayElement;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(_device->GetRawDevice(), 1, &descriptorWrite, 0, nullptr);
}

void VulkanDescriptorSet::Update(uint32_t binding, uint32_t arrayElement, const VkDescriptorImageInfo& imageInfo)
{
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = _vkDescriptorSet;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = arrayElement;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(_device->GetRawDevice(), 1, &descriptorWrite, 0, nullptr);
}

const VkDescriptorSet& VulkanDescriptorSet::GetRawDescriptorSet()
{
    return _vkDescriptorSet;
}

} // namespace ZE