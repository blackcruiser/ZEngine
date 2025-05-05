#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanDevice;
class VulkanImageView;
class VulkanRenderPass;

class VulkanFramebuffer : public VulkanDeviceChild
{
public:
    VulkanFramebuffer(VulkanDevice* device, VulkanRenderPass* renderPass, const std::vector<VulkanImageView*>& imageViewArr, const VkExtent2D& extent);
    ~VulkanFramebuffer();

    VkFramebuffer GetRawFramebuffer();

private:
    VkFramebuffer _framebuffer;

    std::vector<VulkanImageView*> _imageViewArr;
};

} // namespace ZE