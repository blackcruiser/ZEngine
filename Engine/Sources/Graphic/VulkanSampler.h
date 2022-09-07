#pragma once

#include "CoreDefine.h"

#include <vulkan/vulkan.hpp>


namespace TE {

class VulkanDevice;

class VulkanSampler
{
public:
    VulkanSampler(TPtr<VulkanDevice> device);
    ~VulkanSampler();

    VkSampler GetRawSampler();

private:
    VkSampler _vkSampler;

    TPtr<VulkanDevice> _device;
};

} // namespace TE