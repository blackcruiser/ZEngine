#include "RenderingContext.h"
#include "VulkanDevice.h"

namespace ZE {

RenderingContext(TPtr<VulkanDevice> device) :
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

void RenderingContext::BeginRendering()
{
    TPtr<VulkanSwapchain> swapchain = _window->GetSwapchain();
    TPtr<Frame> frame = make_shared<Frame>(RenderSystem::Get().GetDevice(), swapchain);
    TPtr<VulkanCommandBuffer> commandBuffer = frame->GetCachedCommandBuffer();
}

void RenderingContext::EndRendering()
{
    std::vector<VkSemaphore> waitSemaphores{frame->GetAvailableSemaphore()};
    std::vector<VkPipelineStageFlags> waitStages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSemaphore submitSemaphore = RenderSystem::Get().GetDevice()->CreateGraphicSemaphore();
    frame->PutSemaphore(submitSemaphore);

    TPtr<VulkanQueue> graphicQueue = RenderSystem::Get().GetQueue(VulkanQueue::EType::Graphic);
    graphicQueue->Submit(commandBuffer, waitSemaphores, waitStages, {submitSemaphore}, frame->GetFence());
    graphicQueue->Present(swapchain, {submitSemaphore});

    if (frame)
    {
        VkFence fence = frame->GetFence();
        vkWaitForFences(RenderSystem::Get().GetDevice()->GetRawDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
    }
    LastFrame = frame;

    RenderSystem::Get().GetBufferManager()->Tick();
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