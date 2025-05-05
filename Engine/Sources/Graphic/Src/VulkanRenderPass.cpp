#include "VulkanRenderPass.h"
#include "Misc/AssertionMacros.h"


namespace ZE {

VulkanRenderPass::VulkanRenderPass(VulkanDevice* device, const std::vector<VkAttachmentDescription>& colorAttachmentDescriptionArr, const VkAttachmentDescription& depthAttachment)
    : VulkanDeviceChild(device), _renderPass(VK_NULL_HANDLE)
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

    VkResult result = vkCreateRenderPass(_device->GetRawDevice(), &renderPassInfo, nullptr, &_renderPass);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create RenderPass!")
}

VulkanRenderPass::VulkanRenderPass(VulkanDevice* device, const std::vector<VkAttachmentDescription>& colorAttachmentDescriptionArr)
    : VulkanDeviceChild(device), _renderPass(VK_NULL_HANDLE)
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

    VkResult result = vkCreateRenderPass(_device->GetRawDevice(), &renderPassInfo, nullptr, &_renderPass);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create RenderPass!")
}

VulkanRenderPass::VulkanRenderPass(VulkanDevice* device, const VkAttachmentDescription& depthAttachment)
    : VulkanDeviceChild(device), _renderPass(VK_NULL_HANDLE)
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

    VkResult result = vkCreateRenderPass(_device->GetRawDevice(), &renderPassInfo, nullptr, &_renderPass);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create RenderPass!")
}

VulkanRenderPass::~VulkanRenderPass()
{
    CHECK(_renderPass != VK_NULL_HANDLE);
    vkDestroyRenderPass(_device->GetRawDevice(), _renderPass, nullptr);
}

VkRenderPass VulkanRenderPass::GetRawRenderPass()
{
    return _renderPass;
}

} // namespace ZE
