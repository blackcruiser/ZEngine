#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanQueue.h"

namespace ZE {

class VulkanBuffer;
class VulkanDescriptorPool;
class VulkanCommandBufferManager;
class VulkanBufferManager;
class VulkanGraphicPipeline;
class VulkanRenderPass;
class RenderGraph;

class RenderingContext
{
public:
    RenderingContext(TPtr<VulkanDevice> device);
    ~RenderingContext();

    void Initialize();

    TPtr<VulkanBuffer> AcquireStagingBuffer(uint32_t size);
    TPtr<VulkanBuffer> AcquireBuffer(uint32_t size, VkBufferUsageFlags bits, VkMemoryPropertyFlags properties);
    
    TPtr<VulkanDevice> GetDevice();
    TPtr<VulkanQueue> GetQueue(VulkanQueue::EType type);
    TPtr<VulkanDescriptorPool> GetDescriptorPool();
    TPtr<VulkanCommandBufferManager> GetCommandBufferManager();
    TPtr<VulkanBufferManager> GetBufferManager();
    TPtrSet<VulkanGraphicPipeline>& GetPipelineCache();

private:
    TPtr<VulkanDevice> _device;

    TPtrArr<VulkanQueue> _queueArr;
    TPtr<VulkanDescriptorPool> _descriptorPool;
    TPtr<VulkanCommandBufferManager> _commandBufferManager;
    TPtr<VulkanBufferManager> _bufferManager;
    TPtrSet<VulkanGraphicPipeline> _pipelineCache;

    TPtrArr<VulkanRenderPass> _renderPassArr;
};

}