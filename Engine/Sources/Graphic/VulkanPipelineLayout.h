#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanDescriptorSetLayout;

class VulkanPipelineLayout : public VulkanDeviceChild
{
public:
    VulkanPipelineLayout(VulkanDevice* device, std::vector<VulkanDescriptorSetLayout*>& descriptorSetLayoutArr);
    ~VulkanPipelineLayout();

    VkPipelineLayout GetRawPipelineLayout();

private:
    VkPipelineLayout _pipelineLayout;

    std::vector<VulkanDescriptorSetLayout*> _descriptorSetLayoutArr;
};

} // namespace ZE