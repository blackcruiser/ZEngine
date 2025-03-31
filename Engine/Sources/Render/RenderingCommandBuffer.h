#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

namespace ZE {

class VulkanImage;
class VulkanDescriptorPool;
class VulkanCommandBufferManager;
class VulkanBufferManager;
class VulkanGraphicPipeline;
class VulkanRenderPass;

class RenderingCommandBuffer
{
public:
    RenderingCommandBuffer(TPtr<VulkanDevice> device);
    virtual ~RenderingCommandBuffer();

    void BeginRendering();
    void EndRendering();

    void SetRenderTargets(TPtr<RenderTargets> renderTargets);

    TPtr<VulkanImage> CreateImage(const VkExtent3D& extent, VkFormat format, VkImageUsageFlags usageFlags);

private:
    TPtr<VulkanDevice> _device;

    TPtrArr<VulkanRenderPass> _renderPassArr;
};

}