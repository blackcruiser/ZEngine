#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

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

} // namespace ZE
