#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDevice.h"


namespace TE {

VulkanDescriptorSet::VulkanDescriptorSet(TPtr<VulkanDescriptorPool> descriptorPool, const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings)
    : _descriptorPool(descriptorPool), _vkDescriptorSetLayout(VK_NULL_HANDLE), _vkDescriptorSet(VK_NULL_HANDLE)
{
    TPtr<VulkanDevice> vulkanDevice = _descriptorPool->GetDevice();
    VkDevice vkDevice = vulkanDevice->GetRawDevice();

    // Layout
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, nullptr, &_vkDescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    // Allocate
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool->GetRawDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &_vkDescriptorSetLayout;

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
    vkDestroyDescriptorSetLayout(vkDevice, _vkDescriptorSetLayout, nullptr);
}

void VulkanDescriptorSet::Update(const std::vector<VkDescriptorBufferInfo>& bufferInfoArr, const std::vector<VkDescriptorImageInfo>& imageInfoArr)
{
    std::vector<VkWriteDescriptorSet> descriptorWriteArr;

    if (bufferInfoArr.size() > 0)
    {
        VkWriteDescriptorSet uniformBufferDescriptorWrite{};
        uniformBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformBufferDescriptorWrite.dstSet = _vkDescriptorSet;
        uniformBufferDescriptorWrite.dstBinding = 0;
        uniformBufferDescriptorWrite.dstArrayElement = 0;
        uniformBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferDescriptorWrite.descriptorCount = bufferInfoArr.size();;
        uniformBufferDescriptorWrite.pBufferInfo = bufferInfoArr.data();

        descriptorWriteArr.push_back(uniformBufferDescriptorWrite);
    }

    if (imageInfoArr.size() > 0)
    {
        VkWriteDescriptorSet samplerBufferDescriptorWrite{};
        samplerBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        samplerBufferDescriptorWrite.dstSet = _vkDescriptorSet;
        samplerBufferDescriptorWrite.dstBinding = 1;
        samplerBufferDescriptorWrite.dstArrayElement = 0;
        samplerBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerBufferDescriptorWrite.descriptorCount = imageInfoArr.size();
        samplerBufferDescriptorWrite.pImageInfo = imageInfoArr.data();

        descriptorWriteArr.push_back(samplerBufferDescriptorWrite);
    }

    vkUpdateDescriptorSets(_descriptorPool->GetDevice()->GetRawDevice(), static_cast<uint32_t>(descriptorWriteArr.size()), descriptorWriteArr.data(), 0, nullptr);
}

const VkDescriptorSet& VulkanDescriptorSet::GetRawDescriptorSet()
{
    return _vkDescriptorSet;
}

const VkDescriptorSetLayout& VulkanDescriptorSet::GetRawDescriptorSetLayout()
{
    return _vkDescriptorSetLayout;
}
} // namespace TE