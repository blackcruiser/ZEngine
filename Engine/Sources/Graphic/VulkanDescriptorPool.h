#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.h>


namespace TE {

class VulkanDevice;

class VulkanDescriptorPool
{
public:
    VulkanDescriptorPool(TPtr<VulkanDevice> device, const std::vector<VkDescriptorPoolSize>& descriptorPoolSizeArr);
    ~VulkanDescriptorPool();

    TPtr<VulkanDevice> GetDevice();

    VkDescriptorPool GetRawDescriptorPool();

private:
    VkDescriptorPool _descriptorPool;

    TPtr<VulkanDevice> _device;
};

} // namespace TE