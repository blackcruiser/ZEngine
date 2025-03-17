#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDescriptorPool;


class VulkanDescriptorSet
{
public:
public:
    VulkanDescriptorSet(TPtr<VulkanDescriptorPool> descriptorPool, const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings);
    ~VulkanDescriptorSet();

    void Update(const std::vector<VkDescriptorBufferInfo>& bufferInfoArr, const std::vector<VkDescriptorImageInfo>& ImageInfoArr);

    const VkDescriptorSet& GetRawDescriptorSet();
    const VkDescriptorSetLayout& GetRawDescriptorSetLayout();

private:
    VkDescriptorSetLayout _vkDescriptorSetLayout;
    VkDescriptorSet _vkDescriptorSet;

    TPtr<VulkanDescriptorPool> _descriptorPool;
};

} // namespace ZE