#include "RenderPass.h"
#include "RenderTargets.h"
#include "Frame.h"
#include "Graphic/VulkanCommandBuffer.h"
#include "Graphic/VulkanRenderPass.h"
#include "Graphic/VulkanFramebuffer.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanImageView.h"
#include "Graphic/VulkanBuffer.h"

namespace ZE {

RenderPass::RenderPass()
    : _renderPass(nullptr)
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::Execute(TPtrArr<SceneObject> objectsToRender, TPtr<VulkanCommandBuffer> commandBuffer, TPtr<Frame> frame, TPtr<RenderTargets> renderTargets)
{
    TPtr<VulkanDevice> device = commandBuffer->GetDevice();
    VkExtent3D extent3D = renderTargets->colors.empty() ? renderTargets->depthStencil.value().target->GetExtent() : renderTargets->colors[0].target->GetExtent();
    VkExtent2D extent2D{extent3D.width, extent3D.height};

    TPtrArr<VulkanImageView> framebufferImageArr;
    std::vector<VkAttachmentDescription> colorAttachmentArr;
    std::vector<VkClearValue> clearValues;

    for (auto& bindings : renderTargets->colors)
    {
        TPtr<VulkanImageView> image = bindings.target;

        VkAttachmentDescription attachment{};
        attachment.format = image->GetFormat();
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        colorAttachmentArr.emplace_back(attachment);
        framebufferImageArr.emplace_back(image);

        VkClearValue clearValue;
        clearValue.color = {0.0f, 0.0f, 0.0f, 0.0f};
        clearValues.push_back(clearValue);
    }

    if (renderTargets->depthStencil.has_value())
    {
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = VkFormat::VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        _renderPass = std::make_shared<VulkanRenderPass>(device, colorAttachmentArr, depthAttachment);
        framebufferImageArr.emplace_back(renderTargets->depthStencil.value().target);

        VkClearValue clearValue;
        clearValue.depthStencil.depth = 0.0f;
        clearValue.depthStencil.stencil = 0;
        clearValues.push_back(clearValue);
    }
    else
    {
        _renderPass = std::make_shared<VulkanRenderPass>(device, colorAttachmentArr);
    }

    TPtr<VulkanFramebuffer> framebuffer = std::make_shared<VulkanFramebuffer>(device, _renderPass, framebufferImageArr, extent2D);
    frame->PutFramebuffer(framebuffer);
    commandBuffer->BeginRenderPass(_renderPass, framebuffer, {{0, 0}, extent2D}, clearValues);

    VkViewport viewport{0.0f, 0.0f, static_cast<float>(extent2D.width), static_cast<float>(extent2D.height), 0.0f, 1.0f};
    vkCmdSetViewport(commandBuffer->GetRawCommandBuffer(), 0, 1, &viewport);

    VkRect2D scissor{0, 0, static_cast<int32_t>(extent2D.width), static_cast<int32_t>(extent2D.height)};
    vkCmdSetScissor(commandBuffer->GetRawCommandBuffer(), 0, 1, &scissor);

    Draw(objectsToRender, commandBuffer, renderTargets);

    commandBuffer->EndRenderPass();
}

} // namespace ZE