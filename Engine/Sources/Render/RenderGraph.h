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
struct RenderTargets;
struct RHIPipelineState;


class RenderGraph
{
public:
    RenderGraph();
    virtual ~RenderGraph();

    void BeginRenderPass();
    void EndRenderPass();

    void Execute(const std::vector<VkSemaphore>& waitSemaphoreArr, const std::vector<VkPipelineStageFlags>& waitStageArr, const std::vector<VkSemaphore>& signalSemaphoreArr);
    void Execute();

    void CopyBuffer(const uint8_t* data, uint32_t size, TPtr<VulkanBuffer> destination);
    void CopyImage(const uint8_t* data, uint32_t size, TPtr<VulkanImage> destination);

    void TransitionLayout(TPtr<VulkanImage> image, VkImageLayout oldLayout, VkImageLayout newLayout);

    void SetRenderTargets(TPtr<RenderTargets> renderTargets);
    TPtr<VulkanImageView> GetFramebuffer();

    void SetPipelineState(const RHIPipelineState& pipelineState, TPtr<VulkanDescriptorSet> descriptorSet);

    void BindVertexBuffer(TPtr<VulkanBuffer> vertexBuffer, TPtr<VulkanBuffer> indexBuffer);

    void DrawIndexed(uint32_t verticesCount, uint32_t firstIndex);

    TPtr<VulkanCommandBuffer> GetCommandBuffer();
    TPtr<VulkanDevice> GetDevice();

private:
    TPtr<VulkanDevice> _device;

    TPtr<VulkanCommandBuffer> _commandBuffer;
    TPtr<RenderTargets> _pendingRenderTargets;
    TPtr<VulkanRenderPass> _pendingRenderPass;
};

}