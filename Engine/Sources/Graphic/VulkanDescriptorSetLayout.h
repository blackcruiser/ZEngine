#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanDescriptorSetLayout : public VulkanDeviceChild
{
public:
    VulkanDescriptorSetLayout(VulkanDevice* device, const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings);
    ~VulkanDescriptorSetLayout();

    VkDescriptorSetLayout GetRawDescriptorSetLayout();

private:
    VkDescriptorSetLayout _descriptorSetLayout;
};

} // namespace ZE