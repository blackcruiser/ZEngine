#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.h>


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
