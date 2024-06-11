#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"

#include <stdexcept>


namespace ZE {

VulkanCommandBuffer::VulkanCommandBuffer(TPtr<VulkanCommandPool> commandPool)
    : _commandPool(commandPool), _vkCommandBuffer(VK_NULL_HANDLE), _vkFence(VK_NULL_HANDLE), _status(EStatus::Initial), _executeCount(0)
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

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = 0;
    if (vkCreateFence(vkDevice, &fenceInfo, nullptr, &_vkFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate fence!");
    }
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    VkDevice vkDevice = _commandPool->GetDevice()->GetRawDevice();
    VkCommandPool vkCommandPool = _commandPool->GetRawCommandPool();

    vkDestroyFence(vkDevice, _vkFence, nullptr);

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
}

void VulkanCommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(_vkCommandBuffer);
}

uint32_t VulkanCommandBuffer::GetExecuteCount()
{
    return _executeCount;
}

VkFence VulkanCommandBuffer::GetFence()
{
    return _vkFence;
}

VkCommandBuffer VulkanCommandBuffer::GetRawCommandBuffer()
{
    return _vkCommandBuffer;
}

TPtr<VulkanDevice> VulkanCommandBuffer::GetDevice()
{
    return _commandPool->GetDevice();
}

} // namespace ZE