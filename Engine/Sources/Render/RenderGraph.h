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
class VulkanCommandBufferManager;
class VulkanRenderPass;
class VulkanDescriptorSet;
struct RenderTargets;
struct RHIPipelineState;
class RenderingContext;

class RenderGraph
{
public:
    RenderGraph(TPtr<RenderingContext> renderingContext);
    virtual ~RenderGraph();

    void BeginRendering();
    void EndRendering();

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

private:
    TPtr<VulkanDevice> _device;
    TPtr<VulkanCommandBufferManager> _commandBufferManager;
    TPtr<VulkanCommandBuffer> _commandBuffer;
    TPtr<VulkanRenderPass> _currentRenderPass;
    TPtrArr<VulkanRenderPass> _renderPassArr;

    TPtr<RenderingContext> _renderingContext;
};

}