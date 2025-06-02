#include "VulkanFramebuffer.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanRenderPass.h"
#include "Debug/AssertionMacros.h"

#include <algorithm>
#include <iterator>


namespace ZE {

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* device, VulkanRenderPass* renderPass, const std::vector<VulkanImage*>& imageArr, const VkExtent2D& extent)
    : VulkanDeviceChild(device), _framebuffer(VK_NULL_HANDLE)
{
    std::copy(imageArr.begin(), imageArr.end(), std::back_inserter(_imageArr));
    std::vector<VkImageView> vkImageViewArr;
    std::transform(imageArr.begin(), imageArr.end(), std::back_inserter(vkImageViewArr), [](VulkanImage* image) {
        return image->GetRawImageView();
    });

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass->GetRawRenderPass();
    framebufferInfo.attachmentCount = vkImageViewArr.size();
    framebufferInfo.pAttachments = vkImageViewArr.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    VkResult result = vkCreateFramebuffer(_device->GetRawDevice(), &framebufferInfo, nullptr, &_framebuffer);
    ZE_CHECK_MSG(result == VkResult::VK_SUCCESS, "failed to allocate DescriptorSets!");
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    ZE_CHECK(_framebuffer != VK_NULL_HANDLE);
    vkDestroyFramebuffer(_device->GetRawDevice(), _framebuffer, nullptr);
}


VkFramebuffer VulkanFramebuffer::GetRawFramebuffer()
{
    return _framebuffer;
}


} // namespace ZE
