#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;
class VulkanDescriptorSetLayout;

class VulkanPipelineLayout
{
public:
    VulkanPipelineLayout(TPtr<VulkanDevice> device, TPtrArr<VulkanDescriptorSetLayout>& descriptorSetLayoutArr);
    ~VulkanPipelineLayout();

    VkPipelineLayout GetRawPipelineLayout();

private:
    VkPipelineLayout _vkPipelineLayout;

    TPtrArr<VulkanDescriptorSetLayout> _descriptorSetLayoutArr;
    TPtr<VulkanDevice> _device;
};

} // namespace ZE