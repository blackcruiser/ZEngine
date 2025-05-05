#include "VulkanDescriptorPool.h"
#include "VulkanDevice.h"

#include <stdexcept>


namespace ZE {

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice* device, const std::vector<VkDescriptorPoolSize>& descriptorPoolSizeArr)
    : VulkanDeviceChild(device), _descriptorPool(VK_NULL_HANDLE)
{
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 2;
    poolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizeArr.size());
    poolInfo.pPoolSizes = descriptorPoolSizeArr.data();

    if (vkCreateDescriptorPool(_device->GetRawDevice(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    vkDestroyDescriptorPool(_device->GetRawDevice(), _descriptorPool, nullptr);
}

VkDescriptorPool VulkanDescriptorPool::GetRawDescriptorPool()
{
    return _descriptorPool;
}

} // namespace ZE
