#include "RenderSystem.h"
#include "Graphic/VulkanContext.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanBuffer.h"
#include "Graphic/VulkanStagingBufferManager.h"
#include "Graphic/VulkanCommandBufferManager.h"
#include "Graphic/VulkanDescriptorPool.h"
#include "Graphic/GraphicResource.h"
#include "RenderResource.h"
#include "RenderGraph.h"

#include "Debug/AssertionMacros.h"

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <cassert>


namespace ZE {

RenderSystem& RenderSystem::Get()
{
    static RenderSystem instance;

    return instance;
}

RenderSystem::RenderSystem()
    : _instance(nullptr), _device(nullptr)
{
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Initialize()
{
    _instance = CreateInstance();
    _physicalDevice = AcquirePhysicalDevice(_instance);
    _device = new VulkanDevice(_instance, _physicalDevice);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties = GetQueueFamilyProperties(_physicalDevice);
    for (size_t i = 0; i < queueFamilyProperties.size(); i++)
    {
        VkQueueFamilyProperties properties = queueFamilyProperties[i];

        if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
        {
            _graphicQueue = new VulkanQueue(_device, VulkanQueue::EType::Graphic, i);
            _graphicCommandBufferManager = new VulkanCommandBufferManager(_device, i);
            break;
        }

        // if ((properties.queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT)
        // {
        //     _computeQueue = new VulkanQueue(_device, VulkanQueue::EType::Compute, i);
        //     _computeCommandBufferManager = new VulkanCommandBufferManager(_device, i);
        // }

        // if ((properties.queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT)
        // {
        //     _transferQueue = new VulkanQueue(_device, VulkanQueue::EType::Transfer, i);
        //     _transferCommandBufferManager = new VulkanCommandBufferManager(_device, i);
        // }
    }

    std::vector<VkDescriptorPoolSize> poolSizeArr = {{VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10}, {VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}};
    _descriptorPool = new VulkanDescriptorPool(_device, poolSizeArr);

    _bufferManager = new VulkanStagingBufferManager(_device);

    _renderGraph = new RenderGraph(_device);
}

void RenderSystem::Cleanup()
{
    delete _renderGraph;

    delete _bufferManager;
    delete _graphicCommandBufferManager;
    delete _computeCommandBufferManager;
    delete _transferCommandBufferManager;
    delete _descriptorPool;

    delete _graphicQueue;
    delete _computeQueue;
    delete _transferQueue;

    delete _device;

    _physicalDevice = VK_NULL_HANDLE;
    DestroyInstance(_instance);
}

void RenderSystem::InitializeResources(RenderGraph* renderGraph)
{
    RenderResource::InitializeRenderResources(renderGraph);
}

void RenderSystem::CleanupResources(RenderGraph* renderGraph)
{
    _device->WaitIdle();

    RenderResource::CleanupRenderResources(renderGraph);
    renderGraph->GarbageCollect(renderGraph->GetExecuteCounter());
}

void RenderSystem::DeleteGraphicResources()
{
    std::vector<GraphicResource*>& resources = GraphicResource::GetPendingDeleteResources();
    for (GraphicResource* resource : resources)
    {
        
    }
}

void RenderSystem::Tick()
{
}

VulkanDevice* RenderSystem::GetDevice()
{
    return _device;
}

VulkanQueue* RenderSystem::GetQueue(VulkanQueue::EType type)
{
    switch (type)
    {
    case VulkanQueue::EType::Graphic:
        return _graphicQueue;
    case VulkanQueue::EType::Compute:
        return _computeQueue;
    case VulkanQueue::EType::Transfer:
        return _transferQueue;
    }

    return nullptr;
}

VulkanDescriptorPool* RenderSystem::GetDescriptorPool()
{
    return _descriptorPool;
}

VulkanCommandBufferManager* RenderSystem::GetCommandBufferManager(VulkanQueue::EType type)
{
    switch (type)
    {
    case VulkanQueue::EType::Graphic:
        return _graphicCommandBufferManager;
    case VulkanQueue::EType::Compute:
        return _computeCommandBufferManager;
    case VulkanQueue::EType::Transfer:
        return _transferCommandBufferManager;
    }

    return nullptr;
}

VulkanStagingBufferManager* RenderSystem::GetBufferManager()
{
    return _bufferManager;
}

RenderGraph* RenderSystem::GetRenderGraph(uint32 threadId)
{
    return _renderGraph;
}

} // namespace ZE