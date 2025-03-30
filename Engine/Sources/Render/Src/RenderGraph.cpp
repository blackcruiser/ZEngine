#include "RenderGraph.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanCommandBuffer.h"
#include "Graphic/VulkanCommandBufferManager.h"
#include "Graphic/VulkanBuffer.h"
#include "Graphic/VulkanBufferManager.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanImageView.h"
#include "Graphic/VulkanPipeline.h"
#include "Graphic/VulkanDescriptorSet.h"
#include "Graphic/VulkanFramebuffer.h"
#include "Graphic/VulkanRenderPass.h"
#include "Render/RenderSystem.h"
#include "Render/RenderTargets.h"

#include <stdexcept>


namespace ZE {

RenderGraph::RenderGraph()
{
    _device = RenderSystem::Get().GetDevice();
    _commandBuffer = RenderSystem::Get().GetCommandBufferManager()->Acquire(VulkanQueue::EType::Graphic);
    _commandBuffer->Begin();
}

RenderGraph::~RenderGraph()
{
    _commandBuffer->End();
    RenderSystem::Get().GetCommandBufferManager()->Release(_commandBuffer);
}

void RenderGraph::Execute(const std::vector<VkSemaphore>& waitSemaphoreArr, const std::vector<VkPipelineStageFlags>& waitStageArr, const std::vector<VkSemaphore>& signalSemaphoreArr)
{
    _commandBuffer->End();
    TPtr<VulkanQueue> graphicQueue = RenderSystem::Get().GetQueue(VulkanQueue::EType::Graphic);
    graphicQueue->Submit(_commandBuffer, waitSemaphoreArr, waitStageArr, signalSemaphoreArr, _commandBuffer->GetFence());

    RenderSystem::Get().GetCommandBufferManager()->Release(_commandBuffer);
    _commandBuffer = RenderSystem::Get().GetCommandBufferManager()->Acquire(VulkanQueue::EType::Graphic);
    _commandBuffer->Begin();
}

void RenderGraph::CopyBuffer(const uint8_t* data, uint32_t size, TPtr<VulkanBuffer> destination)
{
    VkMemoryPropertyFlags properties = destination->GetProperties();

    if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        void* address = destination->MapMemory(0, size);
        memcpy(address, data, size);
        destination->UnmapMemory();
    }
    else if (properties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    {
        TPtr<VulkanBuffer> stagingBuffer = RenderSystem::Get().GetBufferManager()->AcquireStagingBuffer(size);
    
        void* mappedAddress = stagingBuffer->MapMemory(0, size);
        memcpy(mappedAddress, data, size);
        stagingBuffer->UnmapMemory();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(_commandBuffer->GetRawCommandBuffer(), stagingBuffer->GetRawBuffer(), destination->GetRawBuffer(), 1, &copyRegion);

        RenderSystem::Get().GetBufferManager()->ReleaseStagingBuffer(stagingBuffer, _commandBuffer);
    }
}

void RenderGraph::TransitionLayout(TPtr<VulkanImage> image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image->GetRawImage();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        barrier.srcAccessMask = VK_ACCESS_NONE;
        barrier.dstAccessMask = VK_ACCESS_NONE;

        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(_commandBuffer->GetRawCommandBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    image->SetLayout(newLayout);
}

void RenderGraph::CopyImage(const uint8_t* data, uint32_t size, TPtr<VulkanImage> destination)
{
    TPtr<VulkanBuffer> stagingBuffer = RenderSystem::Get().GetBufferManager()->AcquireStagingBuffer(size);

    void* mappedAddress = stagingBuffer->MapMemory(0, size);
    memcpy(mappedAddress, data, size);
    stagingBuffer->UnmapMemory();

    TransitionLayout(destination, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = VkOffset3D();
    region.imageExtent = destination->GetExtent();

    vkCmdCopyBufferToImage(_commandBuffer->GetRawCommandBuffer(), stagingBuffer->GetRawBuffer(), destination->GetRawImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    TransitionLayout(destination, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    RenderSystem::Get().GetBufferManager()->ReleaseStagingBuffer(stagingBuffer, _commandBuffer);
}

VkAttachmentLoadOp ConvertRenderTargetLoadActionToVulkan(ERenderTargetLoadAction loadAction)
{
    switch (loadAction)
    {
    case ERenderTargetLoadAction::Load:
        return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
    case ERenderTargetLoadAction::Clear:
        return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
    case ERenderTargetLoadAction::DontCare:
        return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }

    return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

void RenderGraph::SetRenderTargets(TPtr<RenderTargets> renderTargets)
{
    _pendingRenderTargets = renderTargets;
}

void RenderGraph::SetPipelineState(const RHIPipelineState& pipelineState, TPtr<VulkanDescriptorSet> descriptorSet)
{
    TPtr<VulkanGraphicPipeline> pipeline = std::make_shared<VulkanGraphicPipeline>(_device, pipelineState, _pendingRenderPass);

    vkCmdBindPipeline(_commandBuffer->GetRawCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetRawPipeline());

    vkCmdBindDescriptorSets(_commandBuffer->GetRawCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineState.layout, 0, 1, &descriptorSet->GetRawDescriptorSet(), 0, nullptr);

    _commandBuffer->CachePipeline(pipeline);
}

void RenderGraph::BeginRenderPass()
{
    VkExtent3D extent3D = _pendingRenderTargets->colors.empty() ? _pendingRenderTargets->depthStencil.value().target->GetExtent() : _pendingRenderTargets->colors[0].target->GetExtent();
    VkExtent2D extent2D{extent3D.width, extent3D.height};

    TPtrArr<VulkanImageView> framebufferImageArr;
    std::vector<VkAttachmentDescription> colorAttachmentArr;
    std::vector<VkClearValue> clearValues;

    for (auto& bindings : _pendingRenderTargets->colors)
    {
        TPtr<VulkanImageView> imageView = bindings.target;

        VkAttachmentDescription attachment{};
        attachment.format = imageView->GetImage()->GetFormat();
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = ConvertRenderTargetLoadActionToVulkan(bindings.loadAction);
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = imageView->GetImage()->GetLayout();
        attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        colorAttachmentArr.emplace_back(attachment);
        framebufferImageArr.emplace_back(imageView);

        VkClearValue clearValue;
        clearValue.color = {0.0f, 0.0f, 0.0f, 0.0f};
        clearValues.push_back(clearValue);

        imageView->GetImage()->SetLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }

    TPtr<VulkanRenderPass> renderPass;
    if (_pendingRenderTargets->depthStencil.has_value())
    {
        RenderTargetBinding& depthBinding = _pendingRenderTargets->depthStencil.value();
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = depthBinding.target->GetFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = ConvertRenderTargetLoadActionToVulkan(depthBinding.loadAction);
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = depthBinding.target->GetImage()->GetLayout();
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        renderPass = std::make_shared<VulkanRenderPass>(_device, colorAttachmentArr, depthAttachment);
        framebufferImageArr.emplace_back(_pendingRenderTargets->depthStencil.value().target);

        VkClearValue clearValue;
        clearValue.depthStencil.depth = 0.0f;
        clearValue.depthStencil.stencil = 0;
        clearValues.push_back(clearValue);

        depthBinding.target->GetImage()->SetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    }
    else
    {
        renderPass = std::make_shared<VulkanRenderPass>(_device, colorAttachmentArr);
    }

    TPtr<VulkanFramebuffer> framebuffer = std::make_shared<VulkanFramebuffer>(_device, renderPass, framebufferImageArr, extent2D);
    _commandBuffer->BeginRenderPass(renderPass, framebuffer, {{0, 0}, extent2D}, clearValues);

    VkViewport viewport{0.0f, 0.0f, static_cast<float>(extent2D.width), static_cast<float>(extent2D.height), 0.0f, 1.0f};
    vkCmdSetViewport(_commandBuffer->GetRawCommandBuffer(), 0, 1, &viewport);

    VkRect2D scissor{0, 0, static_cast<int32_t>(extent2D.width), static_cast<int32_t>(extent2D.height)};
    vkCmdSetScissor(_commandBuffer->GetRawCommandBuffer(), 0, 1, &scissor);

    _pendingRenderTargets.reset();
    _pendingRenderPass = renderPass;
}

void RenderGraph::EndRenderPass()
{
    _commandBuffer->EndRenderPass();
}

void RenderGraph::BindVertexBuffer(TPtr<VulkanBuffer> vertexBuffer, TPtr<VulkanBuffer> indexBuffer)
{
    // Vertex Input
    VkBuffer vertexBuffers[] = {vertexBuffer->GetRawBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(_commandBuffer->GetRawCommandBuffer(), 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(_commandBuffer->GetRawCommandBuffer(), indexBuffer->GetRawBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void RenderGraph::DrawIndexed(uint32_t verticesCount, uint32_t firstIndex)
{
    vkCmdDrawIndexed(_commandBuffer->GetRawCommandBuffer(), verticesCount, 1, firstIndex, 0, 0);
}

TPtr<VulkanCommandBuffer> RenderGraph::GetCommandBuffer()
{
    return _commandBuffer;
}

TPtr<VulkanDevice> RenderGraph::GetDevice()
{
    return _device;
}

}