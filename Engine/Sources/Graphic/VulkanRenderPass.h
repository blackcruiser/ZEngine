#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanDevice;

class VulkanRenderPass : public VulkanDeviceChild
{
public:
    VulkanRenderPass(VulkanDevice* device, const std::vector<VkAttachmentDescription>& colorAttachmentDescriptionArr, const VkAttachmentDescription& depthAttachment);
    VulkanRenderPass(VulkanDevice* device, const std::vector<VkAttachmentDescription>& colorAttachmentDescriptionArr);
    VulkanRenderPass(VulkanDevice* device, const VkAttachmentDescription& depthAttachment);
    ~VulkanRenderPass();

    VkRenderPass GetRawRenderPass();

private:
    VkRenderPass _renderPass;
};

} // namespace ZE