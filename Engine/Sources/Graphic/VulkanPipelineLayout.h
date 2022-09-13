#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;
class VulkanDescriptorSet;

class VulkanPipelineLayout
{
public:
    VulkanPipelineLayout(TPtr<VulkanDevice> device, TPtr<VulkanDescriptorSet> descriptorSe);
    ~VulkanPipelineLayout();

    VkPipelineLayout GetRawPipelineLayout();

private:
    VkPipelineLayout _vkPipelineLayout;

    TPtr<VulkanDevice> _device;
};

} // namespace ZE