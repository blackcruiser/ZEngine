#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanDevice;
class VulkanCommandPool;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanGraphicPipeline;

class VulkanCommandBuffer : public VulkanDeviceChild
{
public:
    enum class EStatus : int
    {
        Invalid,
        Initial,
        Recording,
        Executable,
        Pending,
    };

public:
    VulkanCommandBuffer(VulkanDevice* device, VkCommandPool inCommandPool, uint32_t inQueueFamilyIndex);
    ~VulkanCommandBuffer();

    void Begin();
    void End();

    void Reset();

    void BeginRenderPass(VulkanRenderPass* renderPass, VulkanFramebuffer* framebuffer, const VkRect2D& renderArea, const std::vector<VkClearValue>& clearColors);
    void EndRenderPass();

    uint32_t GetExecuteCount();
    VkFence GetFence();

    void CachePipeline(VulkanGraphicPipeline* pipeline);

    VkCommandBuffer GetRawCommandBuffer();
    VkCommandPool GetRawCommandPool();

private:
    VkCommandBuffer _commandBuffer;
    VkCommandPool _commandPool;
    uint32_t _queueFamilyIndex;
    EStatus _status;
    VkFence _fence;
    uint32_t _executeCount;

    std::vector<VulkanRenderPass*> _cachedRenderPasses;
    std::vector<VulkanFramebuffer*> _cachedFramebuffers;
    std::vector<VulkanGraphicPipeline*> _cachedPipelines;
};

} // namespace ZE