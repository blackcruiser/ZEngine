#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>

namespace TE {

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

} // namespace TE