#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "Graphic/VulkanQueue.h"

namespace ZE {

class VulkanGPU;
class VulkanDevice;
class VulkanBuffer;
class VulkanDescriptorPool;
class VulkanCommandBufferManager;
class VulkanBufferManager;
class VulkanGraphicPipeline;
class VulkanRenderPass;

class RenderSystem
{
public:
    static void Initialize();
    static void Cleanup();
    static RenderSystem& Get();

private:
    RenderSystem();
    ~RenderSystem();

    void CreateVulkanInstance();
    void DestroyVulkanInstance();

public:
    void Tick();

    TPtr<VulkanBuffer> AcquireBuffer(uint32_t size, VkBufferUsageFlags bits, VkMemoryPropertyFlags properties);
    
    TPtr<VulkanDevice> GetDevice();
    TPtr<VulkanQueue> GetQueue(VulkanQueue::EType type);
    TPtr<VulkanDescriptorPool> GetDescriptorPool();
    TPtr<VulkanCommandBufferManager> GetCommandBufferManager();
    TPtr<VulkanBufferManager> GetBufferManager();
    TPtrSet<VulkanGraphicPipeline>& GetPipelineCache();


private:
    static RenderSystem* _instance;
    VkInstance _vkInstance;

    TPtr<VulkanGPU> _GPU;
    TPtr<VulkanDevice> _device;

    TPtrArr<VulkanQueue> _queueArr;
    TPtr<VulkanDescriptorPool> _descriptorPool;
    TPtr<VulkanCommandBufferManager> _commandBufferManager;
    TPtr<VulkanBufferManager> _bufferManager;
    TPtrSet<VulkanGraphicPipeline> _pipelineCache;

    TPtrArr<VulkanRenderPass> _renderPassArr;
};

} // namespace ZE