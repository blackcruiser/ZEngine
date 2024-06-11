#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"

#include <stdexcept>


namespace ZE {

VulkanQueue::VulkanQueue(TPtr<VulkanDevice> device, EType type, uint32_t queueFamilyIndex)
    : _device(device), _type(type), _familyIndex(queueFamilyIndex), _vkQueue(VK_NULL_HANDLE)
{
    vkGetDeviceQueue(device->GetRawDevice(), _familyIndex, 0, &_vkQueue);
}

VulkanQueue::~VulkanQueue()
{
}

void VulkanQueue::Submit(TPtr<VulkanCommandBuffer> commandBuffer, const std::vector<VkSemaphore>& waitSemaphoreArr, const std::vector<VkPipelineStageFlags>& waitStageArr, const std ::vector<VkSemaphore>& signalSemaphoreArr, VkFence fence)
{
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkCommandBuffer vkCommandBuffer = commandBuffer->GetRawCommandBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = waitSemaphoreArr.size();
    submitInfo.pWaitSemaphores = waitSemaphoreArr.data();
    submitInfo.pWaitDstStageMask = waitStageArr.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;
    submitInfo.signalSemaphoreCount = signalSemaphoreArr.size();
    submitInfo.pSignalSemaphores = signalSemaphoreArr.data();

    if (vkQueueSubmit(_vkQueue, 1, &submitInfo, fence) != VK_SUCCESS)
    {
       // throw std::runtime_error("failed to submit draw command buffer!");
    }
}

void VulkanQueue::Present(TPtr<VulkanSwapchain> swapchain, const std::vector<VkSemaphore>& waitSemaphoreArr)
{
    VkSwapchainKHR swapchains[] = {swapchain->GetRawSwapchain()};
    uint32_t imageIndex = swapchain->GetCurrentAcquiredIndex();

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = waitSemaphoreArr.size();
    presentInfo.pWaitSemaphores = waitSemaphoreArr.data();
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(_vkQueue, &presentInfo);
}

void VulkanQueue::WaitIdle()
{
    vkQueueWaitIdle(_vkQueue);
}

uint32_t VulkanQueue::GetFamilyIndex()
{
    return _familyIndex;
}

VulkanQueue::EType VulkanQueue::GetType()
{
    return _type;
}

VkQueue VulkanQueue::GetRawQueue()
{
    return _vkQueue;
}


} // namespace ZE
