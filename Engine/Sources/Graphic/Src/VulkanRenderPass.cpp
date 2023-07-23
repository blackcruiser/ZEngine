#include "VulkanRenderPass.h"
#include "VulkanDevice.h"

#include <stdexcept>


namespace ZE {

VulkanRenderPass::VulkanRenderPass(TPtr<VulkanDevice> device, const std::vector<VkAttachmentDescription>& colorAttachmentDescriptionArr, const VkAttachmentDescription& depthAttachment)
    : _device(device), _vkRenderPass(VK_NULL_HANDLE)
{
    std::vector<VkAttachmentDescription> attachmentDescriptionArr;
    attachmentDescriptionArr.insert(attachmentDescriptionArr.begin(), colorAttachmentDescriptionArr.begin(), colorAttachmentDescriptionArr.end());
    attachmentDescriptionArr.push_back(depthAttachment);

    // Reference
    std::vector<VkAttachmentReference> colorAttachmentRefArr;
    for (size_t i = 0; i < colorAttachmentDescriptionArr.size(); i++)
    {
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = static_cast<uint32_t>(i);
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentRefArr.emplace_back(colorAttachmentRef);
    }

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = static_cast<uint32_t>(colorAttachmentDescriptionArr.size());
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Subpass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefArr.size());
    subpass.pColorAttachments = colorAttachmentRefArr.data();
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    // Dependency
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    // Create
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptionArr.size());
    renderPassInfo.pAttachments = attachmentDescriptionArr.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(_device->GetRawDevice(), &renderPassInfo, nullptr, &_vkRenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

VulkanRenderPass::VulkanRenderPass(TPtr<VulkanDevice> device, const std::vector<VkAttachmentDescription>& colorAttachmentDescriptionArr)
    : _device(device), _vkRenderPass(VK_NULL_HANDLE)
{
    // Reference
    std::vector<VkAttachmentReference> colorAttachmentRefArr;
    for (size_t i = 0; i < colorAttachmentDescriptionArr.size(); i++)
    {
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = static_cast<uint32_t>(i);
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentRefArr.emplace_back(colorAttachmentRef);
    }

    // Subpass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefArr.size());
    subpass.pColorAttachments = colorAttachmentRefArr.data();

    // Dependency
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Create
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(colorAttachmentDescriptionArr.size());
    renderPassInfo.pAttachments = colorAttachmentDescriptionArr.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(_device->GetRawDevice(), &renderPassInfo, nullptr, &_vkRenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

VulkanRenderPass::VulkanRenderPass(TPtr<VulkanDevice> device, const VkAttachmentDescription& depthAttachment)
    : _device(device), _vkRenderPass(VK_NULL_HANDLE)
{
    // Reference
    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 0;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Subpass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    // Dependency
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    // Create
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &depthAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(_device->GetRawDevice(), &renderPassInfo, nullptr, &_vkRenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

VulkanRenderPass::~VulkanRenderPass()
{
    if (_vkRenderPass != VK_NULL_HANDLE)
        vkDestroyRenderPass(_device->GetRawDevice(), _vkRenderPass, nullptr);
}


VkRenderPass VulkanRenderPass::GetRawRenderPass()
{
    return _vkRenderPass;
}


} // namespace ZE
