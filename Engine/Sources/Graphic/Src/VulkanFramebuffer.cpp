#include "VulkanFramebuffer.h"
#include "VulkanDevice.h"
#include "VulkanImageView.h"
#include "VulkanRenderPass.h"


namespace TE {


VulkanFramebuffer::VulkanFramebuffer(TPtr<VulkanDevice> device, TPtr<VulkanRenderPass> renderPass, const TPtrArr<VulkanImageView>& imageViewArr, const VkExtent2D& extent)
    : _device(device), _vkFramebuffer(VK_NULL_HANDLE)
{
    std::copy(imageViewArr.begin(), imageViewArr.end(), std::back_inserter(_imageViewArr));
    std::vector<VkImageView> vkImageViewArr;
    std::transform(imageViewArr.begin(), imageViewArr.end(), std::back_inserter(vkImageViewArr), [](TPtr<VulkanImageView> imageView) {
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

    if (vkCreateFramebuffer(_device->GetRawDevice(), &framebufferInfo, nullptr, &_vkFramebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create framebuffer!");
    }
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    if (_vkFramebuffer != VK_NULL_HANDLE)
        vkDestroyFramebuffer(_device->GetRawDevice(), _vkFramebuffer, nullptr);
}


VkFramebuffer VulkanFramebuffer::GetRawFramebuffer()
{
    return _vkFramebuffer;
}


} // namespace TE
