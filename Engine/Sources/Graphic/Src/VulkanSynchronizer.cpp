#include "VulkanSynchronizer.h"
#include "VulkanDevice.h"
#include "Misc/AssertionMacros.h"


namespace ZE {

VkSemaphore CreateSemaphore(VulkanDevice* device)
{
    VkSemaphore semaphore;

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result = vkCreateSemaphore(device->GetRawDevice(), &createInfo, nullptr, &semaphore);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create semaphore!");

    return semaphore;
}

void DestroySemaphore(VulkanDevice* device, VkSemaphore semaphore)
{
    vkDestroySemaphore(device->GetRawDevice(), semaphore, nullptr);
}
    
VkFence CreateFence(VulkanDevice* device, bool bSignaled)
{
    VkFence fence;

    VkFenceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.flags = bSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VkResult result = vkCreateFence(device->GetRawDevice(), &createInfo, nullptr, &fence);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create fence!");

    return fence;
}

void ResetFence(VulkanDevice* device, VkFence inFence)
{
    VkFence fence = inFence;
    vkResetFences(device->GetRawDevice(), 1, &fence);
}

void DestroyFence(VulkanDevice* device, VkFence fence)
{
    vkDestroyFence(device->GetRawDevice(), fence, nullptr);
}

bool IsSignaled(VulkanDevice* device, VkFence fence)
{
    VkResult result = vkGetFenceStatus(device->GetRawDevice(), fence);
    return result == VkResult::VK_SUCCESS;
}

}