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
class VulkanStagingBufferManager;
class VulkanGraphicPipeline;
class VulkanRenderPass;
class GraphicResourceDeleter;

class RenderSystem
{
public:
    static RenderSystem& Get();

private:
    RenderSystem();
    ~RenderSystem();

public:
    void Initialize();
    void Cleanup();

    void InitializeResources();
    void CleanupResources();

public:
    void Tick();
    
    VulkanDevice* GetDevice();
    VulkanQueue* GetQueue(VulkanQueue::EType type);
    VulkanDescriptorPool* GetDescriptorPool();
    VulkanCommandBufferManager* GetCommandBufferManager(VulkanQueue::EType type);
    VulkanStagingBufferManager* GetBufferManager();

    GraphicResourceDeleter* GetResourceDeleter();

private:
    VkInstance _instance;
    VkPhysicalDevice _physicalDevice;

    VulkanDevice* _device;
    VulkanQueue* _graphicQueue;
    VulkanQueue* _computeQueue;
    VulkanQueue* _transferQueue;

    VulkanDescriptorPool* _descriptorPool;
    VulkanCommandBufferManager* _graphicCommandBufferManager;
    VulkanCommandBufferManager* _computeCommandBufferManager;
    VulkanCommandBufferManager* _transferCommandBufferManager;
    VulkanStagingBufferManager* _bufferManager;
    
    GraphicResourceDeleter* _resourceDeleter;
};

} // namespace ZE