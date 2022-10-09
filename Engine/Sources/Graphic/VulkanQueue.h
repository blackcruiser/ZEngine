#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;
class VulkanCommandBuffer;
class VulkanSwapchain;

class VulkanQueue
{
public:
    enum class EType : int
    {
        Unknow,
        Graphic,
        Compute,
        Transfer,
    };


public:
    VulkanQueue(TPtr<VulkanDevice> device, EType type, uint32_t queueFamilyIndex);
    ~VulkanQueue();

    void Submit(TPtr<VulkanCommandBuffer> commandBuffer, const std::vector<VkSemaphore>& waitSemaphoreArr, const std::vector<VkPipelineStageFlags>& waitStageArr, const std ::vector<VkSemaphore>& signalSemaphoreArr, VkFence fence);
    void Present(TPtr<VulkanSwapchain> swapchain, const std::vector<VkSemaphore>& waitSemaphoreArr);

    void WaitIdle();

    uint32_t GetFamilyIndex();
    EType GetType();

    VkQueue GetRawQueue();

private:
    uint32_t _familyIndex;
    EType _type;
    VkQueue _vkQueue;

    TPtr<VulkanDevice> _device;
};

} // namespace ZE