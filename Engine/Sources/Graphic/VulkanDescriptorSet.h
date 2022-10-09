#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDescriptorPool;
class VulkanDescriptorSetLayout;


class VulkanDescriptorSet
{
public:
public:
    VulkanDescriptorSet(TPtr<VulkanDescriptorPool> descriptorPool, TPtr<VulkanDescriptorSetLayout> descriptorSetLayout);
    ~VulkanDescriptorSet();

    void Update(uint32_t binding, uint32_t arrayElement, const VkDescriptorBufferInfo& bufferInfo);
    void Update(uint32_t binding, uint32_t arrayElement, const VkDescriptorImageInfo& imageInfo);

    const VkDescriptorSet& GetRawDescriptorSet();

private:
    VkDescriptorSet _vkDescriptorSet;

    TPtr<VulkanDescriptorSetLayout> _descriptorSetLayout;
    TPtr<VulkanDescriptorPool> _descriptorPool;
};

} // namespace ZE