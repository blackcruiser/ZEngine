#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>

namespace ZE {

class VulkanDevice;

class VulkanRenderPass
{
public:
    VulkanRenderPass(TPtr<VulkanDevice> device, VkFormat format);
    ~VulkanRenderPass();

    VkRenderPass GetRawRenderPass();

private:
    TPtr<VulkanDevice> _device;

    VkRenderPass _vkRenderPass;
};

} // namespace ZE