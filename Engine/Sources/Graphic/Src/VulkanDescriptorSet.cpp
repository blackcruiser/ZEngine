#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDevice.h"

#include <stdexcept>
#include <algorithm>
#include <iterator>


namespace ZE {

VulkanDescriptorSet::VulkanDescriptorSet(TPtr<VulkanDescriptorPool> descriptorPool, TPtr<VulkanDescriptorSetLayout> descriptorSetLayout)
    : _descriptorPool(descriptorPool), _descriptorSetLayout(descriptorSetLayout), _vkDescriptorSet(VK_NULL_HANDLE)
{
    TPtr<VulkanDevice> vulkanDevice = _descriptorPool->GetDevice();
    VkDevice vkDevice = vulkanDevice->GetRawDevice();
    VkDescriptorSetLayout vkDescriptorSetLayout = descriptorSetLayout->GetRawDescriptorSetLayout();


    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool->GetRawDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &vkDescriptorSetLayout;

    if (vkAllocateDescriptorSets(vkDevice, &allocInfo, &_vkDescriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    TPtr<VulkanDevice> vulkanDevice = _descriptorPool->GetDevice();
    VkDevice vkDevice = vulkanDevice->GetRawDevice();

    vkFreeDescriptorSets(vkDevice, _descriptorPool->GetRawDescriptorPool(), 1, &_vkDescriptorSet);
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

    vkUpdateDescriptorSets(_descriptorPool->GetDevice()->GetRawDevice(), 1, &descriptorWrite, 0, nullptr);
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

    vkUpdateDescriptorSets(_descriptorPool->GetDevice()->GetRawDevice(), 1, &descriptorWrite, 0, nullptr);
}

const VkDescriptorSet& VulkanDescriptorSet::GetRawDescriptorSet()
{
    return _vkDescriptorSet;
}

} // namespace ZE