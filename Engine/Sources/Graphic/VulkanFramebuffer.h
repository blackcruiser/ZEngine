#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>

namespace TE {

class VulkanDevice;
class VulkanImageView;
class VulkanRenderPass;

class VulkanFramebuffer
{
public:
    VulkanFramebuffer(TPtr<VulkanDevice> device, TPtr<VulkanRenderPass> renderPass, const TPtrArr<VulkanImageView>& imageViewArr, const VkExtent2D& extent);
    ~VulkanFramebuffer();

    VkFramebuffer GetRawFramebuffer();

private:
    TPtr<VulkanDevice> _device;
    TPtrArr<VulkanImageView> _imageViewArr;

    VkFramebuffer _vkFramebuffer;
};

} // namespace TE