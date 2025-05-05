#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "Graphic/VulkanDevice.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDescriptorPool : public VulkanDeviceChild
{
public:
    VulkanDescriptorPool(VulkanDevice* device, const std::vector<VkDescriptorPoolSize>& descriptorPoolSizeArr);
    ~VulkanDescriptorPool();

    VkDescriptorPool GetRawDescriptorPool();

private:
    VkDescriptorPool _descriptorPool;
};

} // namespace ZE