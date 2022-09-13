#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


namespace ZE {

class VulkanGPU;
class Surface;

class VulkanDevice
{
public:
    VulkanDevice(TPtr<VulkanGPU> GPU, TPtr<Surface> surface);
    ~VulkanDevice();

    VkSemaphore CreateGraphicSemaphore();
    void DestroyGraphicSemaphore(VkSemaphore semaphore);

    VkFence CreateFence(bool isSignaled);
    void DestroyFence(VkFence fence);

    VkDescriptorSetLayout CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings);
    void DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);


    void WaitIdle();

    TPtr<VulkanGPU> GetGPU();

    VkDevice GetRawDevice();
    VkQueue GetGraphicQueue();
    VkQueue GetPresentQueue();

    uint32_t GetGraphicQueueFamilyIndex();
    uint32_t GetPresentQueueFamilyIndex();

private:
    VkDevice _vkDevice;
    VkQueue _vkGraphicQueue, _vkPresentQueue;
    uint32_t _graphicQueueFamilyIndex, _presentQueueFamilyIndex;

    TPtr<VulkanGPU> _GPU;
    TPtr<Surface> _surface;
};

} // namespace ZE