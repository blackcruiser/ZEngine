#pragma once

#include "ZEDefine.h"
#include "ZEType.h"

#include <vulkan/vulkan.h>

namespace ZE {

class VulkanDevice;

class VulkanRenderPass
{
public:
    VulkanRenderPass(TPtr<VulkanDevice> device, const std::vector<VkAttachmentDescription>& colorAttachmentDescriptionArr, const VkAttachmentDescription& depthAttachment);
    VulkanRenderPass(TPtr<VulkanDevice> device, const std::vector<VkAttachmentDescription>& colorAttachmentDescriptionArr);
    VulkanRenderPass(TPtr<VulkanDevice> device, const VkAttachmentDescription& depthAttachment);
    ~VulkanRenderPass();

    VkRenderPass GetRawRenderPass();

private:
    TPtr<VulkanDevice> _device;

    VkRenderPass _vkRenderPass;
};

} // namespace ZE