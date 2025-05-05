#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "Graphic/VulkanDevice.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDescriptorPool;
class VulkanDescriptorSetLayout;


class VulkanDescriptorSet : public VulkanDeviceChild
{
public:
public:
    VulkanDescriptorSet(VulkanDevice* device, VulkanDescriptorPool* descriptorPool, VulkanDescriptorSetLayout* descriptorSetLayout);
    ~VulkanDescriptorSet();

    void Update(uint32_t binding, uint32_t arrayElement, const VkDescriptorBufferInfo& bufferInfo);
    void Update(uint32_t binding, uint32_t arrayElement, const VkDescriptorImageInfo& imageInfo);

    const VkDescriptorSet& GetRawDescriptorSet();

private:
    VkDescriptorSet _vkDescriptorSet;

    VulkanDescriptorSetLayout* _descriptorSetLayout;
    VulkanDescriptorPool* _descriptorPool;
};

} // namespace ZE