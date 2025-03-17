#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>


namespace TE {

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

} // namespace TE