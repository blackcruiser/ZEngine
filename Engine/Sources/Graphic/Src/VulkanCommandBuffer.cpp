#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanSynchronizer.h"
#include "Misc/AssertionMacros.h"

#include <stdexcept>


namespace ZE {

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, VkCommandPool inCommandPool, uint32_t inQueueFamilyIndex)
    : VulkanDeviceChild(device), _commandBuffer(VK_NULL_HANDLE), _commandPool(inCommandPool), _queueFamilyIndex(inQueueFamilyIndex), _status(EStatus::Initial), _fence(VK_NULL_HANDLE), _executeCount(0)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(_device->GetRawDevice(), &allocInfo, &_commandBuffer);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Create CommandBuffer fail!");

    _fence = CreateFence(device, false);
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    DestroyFence(_device, _fence);

    vkFreeCommandBuffers(_device->GetRawDevice(), _commandPool, 1, &_commandBuffer);
}

void VulkanCommandBuffer::Begin()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(_commandBuffer, &beginInfo);

    // ToDo: status and signalCount should match lifecycle
    _status = EStatus::Recording;
}

void VulkanCommandBuffer::End()
{
    vkEndCommandBuffer(_commandBuffer);

    _status = EStatus::Executable;
}

void VulkanCommandBuffer::Reset()
{
    _executeCount++;
    _status = EStatus::Initial;
    ResetFence(_device, _fence);
    

    _cachedRenderPasses.clear();
    _cachedFramebuffers.clear();
    _cachedPipelines.clear();

}

void VulkanCommandBuffer::BeginRenderPass(VulkanRenderPass* renderPass, VulkanFramebuffer* framebuffer, const VkRect2D& renderArea, const std::vector<VkClearValue>& clearColors)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->GetRawRenderPass();
    renderPassInfo.framebuffer = framebuffer->GetRawFramebuffer();
    renderPassInfo.renderArea = renderArea;

    renderPassInfo.clearValueCount = clearColors.size();
    renderPassInfo.pClearValues = clearColors.data();

    vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    _cachedRenderPasses.push_back(renderPass);
    _cachedFramebuffers.push_back(framebuffer);
}

void VulkanCommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(_commandBuffer);
}

void VulkanCommandBuffer::CachePipeline(VulkanGraphicPipeline* pipeline)
{
    _cachedPipelines.push_back(pipeline);
}

uint32_t VulkanCommandBuffer::GetExecuteCount()
{
    return _executeCount;
}

VkFence VulkanCommandBuffer::GetFence()
{
    return _fence;
}

VkCommandBuffer VulkanCommandBuffer::GetRawCommandBuffer()
{
    return _commandBuffer;
}

VkCommandPool VulkanCommandBuffer::GetRawCommandPool()
{
    return _commandPool;
}

} // namespace ZE