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

    void CopyBuffer(const uint8_t* data, uint32_t size, VulkanBuffer* destination);
    void CopyImage(const uint8_t* data, uint32_t size, VulkanImage* destination);

    void TransitionLayout(VulkanImage* image, VkImageLayout oldLayout, VkImageLayout newLayout);

    void SetRenderTargets(TPtr<RenderTargets> renderTargets);
    VulkanImageView* GetFramebuffer();

    void SetPipelineState(const RHIPipelineState& pipelineState, VulkanDescriptorSet* descriptorSet);

    void BindVertexBuffer(VulkanBuffer* vertexBuffer, VulkanBuffer* indexBuffer);

    void DrawIndexed(uint32_t verticesCount, uint32_t firstIndex);

    VulkanCommandBuffer* GetCommandBuffer();
    VulkanDevice* GetDevice();

private:
    VulkanDevice* _device;

    VulkanCommandBuffer* _commandBuffer;
    TPtr<RenderTargets> _pendingRenderTargets;
    VulkanRenderPass* _pendingRenderPass;
};

}