#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanSynchronizer.h"
#include "Debug/AssertionMacros.h"

#include <stdexcept>


namespace ZE {

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, VkCommandPool inCommandPool, uint32_t inQueueFamilyIndex)
    : VulkanDeviceChild(device), _commandBuffer(VK_NULL_HANDLE), _commandPool(inCommandPool), _queueFamilyIndex(inQueueFamilyIndex), _status(EStatus::Initial)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(_device->GetRawDevice(), &allocInfo, &_commandBuffer);
    ZE_CHECK_MSG(result == VkResult::VK_SUCCESS, "Create CommandBuffer fail!");
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    Reset();

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
    _status = EStatus::Initial;
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
}

void VulkanCommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(_commandBuffer);
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