#include "RenderSynchronizer.h"


namespace ZE {

RenderSynchronizer::RenderSynchronizer
{
}

RenderSynchronizer::~RenderSynchronizer
{
    vkDestroyFence(device->GetRawDevice(), fence, nullptr);
}

VkFence RenderSynchronizer::CreateFence(VulkanDevice* device, bool bSignaled)
{
    if (m_freeFences.empty())
    {
        VkFence fence;

        VkFenceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags = bSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        VkResult result = vkCreateFence(device->GetRawDevice(), &createInfo, nullptr, &fence);
        ZE_CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create fence!");

        return fence;
    }
    else
    {
        VkFence fence = m_freeFences.

        return fence;
    }
}

void RenderSynchronizer::MarkExecuted(VkFence fence, uint32 frameNumber)
{
    for (auto iter = m_pendingFences.begin(); iter != m_pendingFences.end(); iter++)
    {
            //VkFence fence = inFence;
            //vkResetFences(device->GetRawDevice(), 1, &fence);

                //VkResult result = vkGetFenceStatus(device->GetRawDevice(), fence);
        //return result == VkResult::VK_SUCCESS;
    }

    m_pendingFences.emplace_back(std::tuple<VkFence, uint32>(fence, frameNumber));
}

}