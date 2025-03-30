#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"

#include <stdexcept>


namespace ZE {

VulkanCommandBuffer::VulkanCommandBuffer(TPtr<VulkanCommandPool> commandPool)
    : _commandPool(commandPool), _vkCommandBuffer(VK_NULL_HANDLE), _fence(VK_NULL_HANDLE), _status(EStatus::Initial), _executeCount(0)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool->GetRawCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    _vkCommandBuffer;
    VkDevice vkDevice = _commandPool->GetDevice()->GetRawDevice();
    if (vkAllocateCommandBuffers(vkDevice, &allocInfo, &_vkCommandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    _fence = _commandPool->GetDevice()->CreateFence(false);
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    _commandPool->GetDevice()->DestroyFence(_fence);

    VkDevice vkDevice = _commandPool->GetDevice()->GetRawDevice();
    VkCommandPool vkCommandPool = _commandPool->GetRawCommandPool();

    vkFreeCommandBuffers(vkDevice, vkCommandPool, 1, &_vkCommandBuffer);
}

void VulkanCommandBuffer::Begin()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(_vkCommandBuffer, &beginInfo);

    // ToDo: status and signalCount should match lifecycle
    _status = EStatus::Recording;
    _executeCount++;
}

void VulkanCommandBuffer::End()
{
    vkEndCommandBuffer(_vkCommandBuffer);

    _status = EStatus::Executable;
}

void VulkanCommandBuffer::Reset()
{
    VkDevice vkDevice = _commandPool->GetDevice()->GetRawDevice();
    _cachedRenderPasses.clear();
    _cachedFramebuffers.clear();
    _cachedPipelines.clear();
    vkResetFences(vkDevice, 1, &_fence);
}

void VulkanCommandBuffer::BeginRenderPass(TPtr<VulkanRenderPass> renderPass, TPtr<VulkanFramebuffer> framebuffer, const VkRect2D& renderArea, const std::vector<VkClearValue>& clearColors)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->GetRawRenderPass();
    renderPassInfo.framebuffer = framebuffer->GetRawFramebuffer();
    renderPassInfo.renderArea = renderArea;

    renderPassInfo.clearValueCount = clearColors.size();
    renderPassInfo.pClearValues = clearColors.data();

    vkCmdBeginRenderPass(_vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    _cachedRenderPasses.push_back(renderPass);
    _cachedFramebuffers.push_back(framebuffer);
}

void VulkanCommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(_vkCommandBuffer);
}

void VulkanCommandBuffer::CachePipeline(TPtr<VulkanGraphicPipeline> pipeline)
{
    _cachedPipelines.push_back(pipeline);
}

VkFence VulkanCommandBuffer::GetFence()
{
    return _fence;
}

uint32_t VulkanCommandBuffer::GetExecuteCount()
{
    return _executeCount;
}

VkCommandBuffer VulkanCommandBuffer::GetRawCommandBuffer()
{
    return _vkCommandBuffer;
}

TPtr<VulkanCommandPool> VulkanCommandBuffer::GetCommandPool()
{
    return _commandPool;
}

TPtr<VulkanDevice> VulkanCommandBuffer::GetDevice()
{
    return _commandPool->GetDevice();
}

} // namespace ZE