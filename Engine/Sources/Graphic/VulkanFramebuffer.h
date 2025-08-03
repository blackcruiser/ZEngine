#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanDevice;
class VulkanImage;
class VulkanRenderPass;

class VulkanFramebuffer : public VulkanDeviceChild
{
public:
    VulkanFramebuffer(VulkanDevice* device, VulkanRenderPass* renderPass, const std::vector<TPtr<VulkanImage>>& imageArr, const VkExtent2D& extent);
    ~VulkanFramebuffer();

    VkFramebuffer GetRawFramebuffer();

private:
    VkFramebuffer _framebuffer;

    std::vector<TPtr<VulkanImage>> _imageArr;
};

} // namespace ZE