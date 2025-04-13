#include "RenderingContext.h"
#include "RenderGraph.h"
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandBufferManager.h"
#include "VulkanBuffer.h"
#include "VulkanBufferManager.h"
#include "VulkanDescriptorPool.h"


namespace ZE {

RenderingContext::RenderingContext(TPtr<VulkanDevice> device) :
    _device(device)
{

}

RenderingContext::~RenderingContext()
{

}

void RenderingContext::Initialize()
{
    TPtr<VulkanQueue> graphicQueue = std::make_shared<VulkanQueue>(_device, VulkanQueue::EType::Graphic, _device->GetGraphicQueueFamilyIndex());
    TPtr<VulkanQueue> computeQueue = std::make_shared<VulkanQueue>(_device, VulkanQueue::EType::Compute, _device->GetComputeQueueFamilyIndex());
    TPtr<VulkanQueue> transferQueue = std::make_shared<VulkanQueue>(_device, VulkanQueue::EType::Transfer, _device->GetTransferQueueFamilyIndex()); 

    _queueArr = {graphicQueue, computeQueue, transferQueue};

    std::vector<VkDescriptorPoolSize> poolSizeArr = {{VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10}, {VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}};
    _descriptorPool = std::make_shared<VulkanDescriptorPool>(_device, poolSizeArr);

    _commandBufferManager = std::make_shared<VulkanCommandBufferManager>(_device, _queueArr);

    _bufferManager = std::make_shared<VulkanBufferManager>(_device);
}

TPtr<VulkanBuffer> RenderingContext::AcquireStagingBuffer(uint32_t size)
{
    return _bufferManager->AcquireStagingBuffer(size);
}

TPtr<VulkanBuffer> RenderingContext::AcquireBuffer(uint32_t size, VkBufferUsageFlags bits, VkMemoryPropertyFlags properties)
{
    return std::make_shared<VulkanBuffer>(_device, size, bits, properties);
}

TPtr<VulkanDevice> RenderingContext::GetDevice()
{
    return _device;
}

TPtr<VulkanQueue> RenderingContext::GetQueue(VulkanQueue::EType type)
{
    size_t index = static_cast<size_t>(type) - 1;
    if (index >= _queueArr.size())
        return nullptr;
    else
        return _queueArr[index];
}

TPtr<VulkanDescriptorPool> RenderingContext::GetDescriptorPool()
{
    return _descriptorPool;
}

TPtr<VulkanCommandBufferManager> RenderingContext::GetCommandBufferManager()
{
    return _commandBufferManager;
}

TPtr<VulkanBufferManager> RenderingContext::GetBufferManager()
{
    return _bufferManager;
}

TPtrSet<VulkanGraphicPipeline>& RenderingContext::GetPipelineCache()
{
    return _pipelineCache;
}

}