#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;
class VulkanCommandPool;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanGraphicPipeline;

class VulkanCommandBuffer
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
    VulkanCommandBuffer(TPtr<VulkanCommandPool> commandPool);
    ~VulkanCommandBuffer();

    void Begin();
    void End();

    void Reset();

    void BeginRenderPass(TPtr<VulkanRenderPass> renderPass, TPtr<VulkanFramebuffer> framebuffer, const VkRect2D& renderArea, const std::vector<VkClearValue>& clearColors);
    void EndRenderPass();

    void CachePipeline(TPtr<VulkanGraphicPipeline> pipeline);

    uint32_t GetExecuteCount();

    VkFence GetFence();
    VkCommandBuffer GetRawCommandBuffer();

    TPtr<VulkanCommandPool> GetCommandPool();
    TPtr<VulkanDevice> GetDevice();

private:
    VkCommandBuffer _vkCommandBuffer;
    VkFence _fence;
    EStatus _status;
    uint32_t _executeCount;

    TPtr<VulkanCommandPool> _commandPool;
    TPtrArr<VulkanRenderPass> _cachedRenderPasses;
    TPtrArr<VulkanFramebuffer> _cachedFramebuffers;
    TPtrArr<VulkanGraphicPipeline> _cachedPipelines;
};

} // namespace ZE