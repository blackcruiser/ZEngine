#include "VulkanFramebuffer.h"
#include "VulkanDevice.h"
#include "VulkanImageView.h"
#include "VulkanRenderPass.h"
#include "Misc/AssertionMacros.h"

#include <algorithm>
#include <iterator>


namespace ZE {

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* device, VulkanRenderPass* renderPass, const std::vector<VulkanImageView*>& imageViewArr, const VkExtent2D& extent)
    : VulkanDeviceChild(device), _framebuffer(VK_NULL_HANDLE)
{
    std::copy(imageViewArr.begin(), imageViewArr.end(), std::back_inserter(_imageViewArr));
    std::vector<VkImageView> vkImageViewArr;
    std::transform(imageViewArr.begin(), imageViewArr.end(), std::back_inserter(vkImageViewArr), [](VulkanImageView* imageView) {
        return imageView->GetRawImageView();
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
    CHECK_MSG(result == VkResult::VK_SUCCESS, "failed to allocate DescriptorSets!");
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    CHECK(_framebuffer != VK_NULL_HANDLE);
    vkDestroyFramebuffer(_device->GetRawDevice(), _framebuffer, nullptr);
}


VkFramebuffer VulkanFramebuffer::GetRawFramebuffer()
{
    return _framebuffer;
}


} // namespace ZE
