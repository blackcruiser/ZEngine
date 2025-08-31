#include "RenderSynchronizer.h"
#include "Graphic/VulkanDevice.h"
#include "Debug/AssertionMacros.h"


namespace ZE {

RenderSynchronizer::RenderSynchronizer(VulkanDevice* device) :
    _device(device), _safeExecuteCounter(0)
{
}

RenderSynchronizer::~RenderSynchronizer()
{
    for (auto& entry : _pendingFences)
    {
        VkFence fence = std::get<0>(entry);
        uint32 executeCounter = std::get<1>(entry);

        vkWaitForFences(_device->GetRawDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
        vkDestroyFence(_device->GetRawDevice(), fence, nullptr);
    }

    for (VkFence fence : _freeFences)
    {
        vkDestroyFence(_device->GetRawDevice(), fence, nullptr);
    }
}

uint32 RenderSynchronizer::getSafeExecuteCounter()
{
    return _safeExecuteCounter;
}

VkFence RenderSynchronizer::GetFence()
{
    VkFence fence = VK_NULL_HANDLE;
    if (_freeFences.empty())
    {
        VkFenceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags = 0;
    
        VkResult result = vkCreateFence(_device->GetRawDevice(), &createInfo, nullptr, &fence);
        ZE_CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create fence!");
    }
    else
    {
        fence = _freeFences.back();
        _freeFences.pop_back();
    }

    return fence;
}

void RenderSynchronizer::ReturnFence(VkFence fence, uint32 executeCounter)
{
    _pendingFences.emplace_back(std::tuple<VkFence, uint32>(fence, executeCounter));
}

void RenderSynchronizer::WaitForAllFences()
{
    for (auto& entry : _pendingFences)
    {
        VkFence fence = std::get<0>(entry);
        vkWaitForFences(_device->GetRawDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
    }
}

void RenderSynchronizer::Recycle()
{
    for (auto iter = _pendingFences.begin(); iter != _pendingFences.end(); )
    {
        VkFence fence = std::get<0>(*iter);
        uint32 executeCounter = std::get<1>(*iter);

        if (executeCounter < _safeExecuteCounter)
        {
            iter = _pendingFences.erase(iter);
        }
        else
        {
            VkResult Result = vkGetFenceStatus(_device->GetRawDevice(), fence);
            if (Result == VK_SUCCESS)
            {
                vkResetFences(_device->GetRawDevice(), 1, &fence);
                _freeFences.push_back(fence);

                _safeExecuteCounter = std::max(_safeExecuteCounter, executeCounter);
                iter = _pendingFences.erase(iter);
            }
            else
            {
                iter ++;
            }
        }
    }
}

}