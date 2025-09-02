#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan_core.h>


namespace ZE {

class VulkanDevice;
class VulkanBuffer;
class VulkanImage;
class VulkanImageView;
class VulkanCommandBuffer;
class VulkanDescriptorSet;
class VulkanRenderPass;
class VulkanSwapchain;
struct RenderTargets;
struct RHIPipelineState;
class RenderSynchronizer;


class RenderGraph
{
public:
    RenderGraph(VulkanDevice* device);
    virtual ~RenderGraph();

    void BeginRenderPass();
    void EndRenderPass();

    void Execute(const std::vector<VkSemaphore>& waitSemaphoreArr, const std::vector<VkPipelineStageFlags>& waitStageArr, const std::vector<VkSemaphore>& signalSemaphoreArr);
    void Execute();

    uint32 GetExecuteCounter();

    RenderSynchronizer* GetSynchronizer();

    void Present(VulkanSwapchain* swapchain, const std::vector<VkSemaphore>& waitSemaphoreArr);

    void GarbageCollect(uint32 safeExecuteCounter);

    void TransferBuffer(const uint8_t* data, uint32_t size, TPtr<VulkanBuffer> destination);
    void TransferImage(const uint8_t* data, uint32_t size, TPtr<VulkanImage> destination);

    void CopyImage(TPtr<VulkanImage> source, TPtr<VulkanImage> destination);

    void TransitionLayout(TPtr<VulkanImage> image, VkImageLayout oldLayout, VkImageLayout newLayout);

    void SetRenderTargets(TPtr<RenderTargets> renderTargets);
    VulkanImageView* GetFramebuffer();

    void SetPipelineState(const RHIPipelineState& pipelineState, VulkanDescriptorSet* descriptorSet);

    void BindVertexBuffer(TPtr<VulkanBuffer> vertexBuffer, TPtr<VulkanBuffer> indexBuffer);

    void DrawIndexed(uint32_t verticesCount, uint32_t firstIndex);

    VulkanCommandBuffer* GetCommandBuffer();
    VulkanDevice* GetDevice();

private:
    VulkanDevice* _device;

    VulkanCommandBuffer* _commandBuffer;
    TPtr<RenderTargets> _pendingRenderTargets;
    VulkanRenderPass* _pendingRenderPass;

    RenderSynchronizer* _synchronizer;

    uint32 _executeCounter;
};

}