#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanCommandBuffer;
class VulkanSwapchain;

class VulkanQueue : public VulkanDeviceChild
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
    VulkanQueue(VulkanDevice* device, EType type, uint32_t queueFamilyIndex);
    ~VulkanQueue();

    void Submit(VulkanCommandBuffer* commandBuffer, const std::vector<VkSemaphore>& waitSemaphoreArr, const std::vector<VkPipelineStageFlags>& waitStageArr, const std ::vector<VkSemaphore>& signalSemaphoreArr, VkFence fence);
    void Present(VulkanSwapchain* swapchain, const std::vector<VkSemaphore>& waitSemaphoreArr);

    void WaitIdle();

    uint32_t GetFamilyIndex();
    EType GetType();

    VkQueue GetRawQueue();

private:
    VkQueue _vkQueue;
    uint32_t _familyIndex;
    EType _type;
};

} // namespace ZE