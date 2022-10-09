#pragma once

#include "ZEDefine.h"
#include "ZEType.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;

class VulkanDescriptorSetLayout
{
public:
    VulkanDescriptorSetLayout(TPtr<VulkanDevice> device, const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings);
    ~VulkanDescriptorSetLayout();

    VkDescriptorSetLayout GetRawDescriptorSetLayout();

private:
    VkDescriptorSetLayout _vkDescriptorSetLayout;

    TPtr<VulkanDevice> _device;
};

} // namespace ZE