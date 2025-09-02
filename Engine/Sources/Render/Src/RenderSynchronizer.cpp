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

    for (auto& entry : _pendingSemaphores)
    {
        VkSemaphore semaphore = std::get<0>(entry);
        vkDestroySemaphore(_device->GetRawDevice(), semaphore, nullptr);
    }

    for (VkSemaphore semaphore : _freeSemaphores)
    {
        vkDestroySemaphore(_device->GetRawDevice(), semaphore, nullptr);
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

void RenderSynchronizer::RecycleFences()
{
    for (auto iter = _pendingFences.begin(); iter != _pendingFences.end(); )
    {
        VkFence fence = std::get<0>(*iter);
        uint32 executeCounter = std::get<1>(*iter);

        if (executeCounter < _safeExecuteCounter || vkGetFenceStatus(_device->GetRawDevice(), fence) == VK_SUCCESS)
        {
            _safeExecuteCounter = std::max(_safeExecuteCounter, executeCounter);

            vkResetFences(_device->GetRawDevice(), 1, &fence);
            _freeFences.push_back(fence);
            iter = _pendingFences.erase(iter);
        }
        else
        {
            iter ++;
        }
    }
}

VkSemaphore RenderSynchronizer::GetSemaphore()
{
    VkSemaphore semaphore = VK_NULL_HANDLE;
    if (_freeSemaphores.empty())
    {
        VkSemaphoreCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
        VkResult result = vkCreateSemaphore(_device->GetRawDevice(), &createInfo, nullptr, &semaphore);
        ZE_CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create semaphore!");
    }
    else
    {
        semaphore = _freeSemaphores.back();
        _freeSemaphores.pop_back();
    }

    return semaphore;
}

void RenderSynchronizer::ReturnSemaphore(VkSemaphore semaphore, uint32 executeCounter)
{
    _pendingSemaphores.emplace_back(std::tuple<VkSemaphore, uint32>(semaphore, executeCounter));
}

void RenderSynchronizer::RecycleSemaphores()
{
    for (auto iter = _pendingSemaphores.begin(); iter != _pendingSemaphores.end(); )
    {
        VkSemaphore semaphore = std::get<0>(*iter);
        uint32 executeCounter = std::get<1>(*iter);

        if (executeCounter <= _safeExecuteCounter)
        {
            _freeSemaphores.push_back(semaphore);
            iter = _pendingSemaphores.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

void RenderSynchronizer::Recycle()
{
    RecycleFences();
    RecycleSemaphores();
}

}