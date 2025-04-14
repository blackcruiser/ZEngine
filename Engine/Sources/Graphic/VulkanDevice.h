#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


namespace ZE {

class VulkanGPU;
class VulkanSurface;

class VulkanDevice
{
public:
    VulkanDevice(TPtr<VulkanGPU> GPU);
    ~VulkanDevice();

    VkSemaphore CreateGraphicSemaphore();
    void DestroyGraphicSemaphore(VkSemaphore semaphore);

    VkFence CreateFence(bool isSignaled = false);
    void DestroyFence(VkFence fence);
    bool IsSignaled(VkFence fence);

    void WaitIdle();

    TPtr<VulkanGPU> GetGPU();

    VkDevice GetRawDevice();

    uint32_t GetGraphicQueueFamilyIndex();
    uint32_t GetComputeQueueFamilyIndex();
    uint32_t GetTransferQueueFamilyIndex();

private:
    VkDevice _vkDevice;
    uint32_t _graphicQueueFamilyIndex, _computeQueueFamilyIndex, _transferQueueFamilyIndex;

    TPtr<VulkanGPU> _GPU;
};

} // namespace ZE