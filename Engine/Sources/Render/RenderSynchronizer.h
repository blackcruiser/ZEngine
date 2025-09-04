#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>

#include <tuple>
#include <vector>


namespace ZE {

class VulkanDevice;

class RenderSynchronizer
{
public:
    RenderSynchronizer(VulkanDevice* device);
    ~RenderSynchronizer();
    
    uint32 getSafeExecuteCounter();

    VkFence GetFence();
    void ReturnFence(VkFence fence, uint32 executeCounter);
    void WaitForFence(VkFence fence);
    void WaitForAllFences();
    void RecycleFences();

    VkSemaphore GetSemaphore();
    void ReturnSemaphore(VkSemaphore semaphore, uint32 executeCounter);
    void RecycleSemaphores();

    void Recycle();

private:
    VulkanDevice* _device;

    uint32 _safeExecuteCounter;

    std::vector<VkFence> _freeFences;
    std::vector<std::tuple<VkFence, uint32>> _pendingFences;

    std::vector<VkSemaphore> _freeSemaphores;
    std::vector<std::tuple<VkSemaphore, uint32>> _pendingSemaphores;
};

} // namespace ZE