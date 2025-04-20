#include "RenderSystem.h"
#include "Graphic/VulkanGPU.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanBuffer.h"
#include "Graphic/VulkanBufferManager.h"
#include "Graphic/VulkanCommandBufferManager.h"
#include "Graphic/VulkanDescriptorPool.h"
#include "RenderSystem.h"
#include "RenderResource.h"
#include "RenderGraph.h"

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
    : _GPU(nullptr), _device(nullptr)
{
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Initialize()
{
    CreateVulkanInstance();

    _GPU = std::make_shared<VulkanGPU>(_vkInstance);
    _device = std::make_shared<VulkanDevice>(_GPU);

    TPtr<VulkanQueue> graphicQueue = std::make_shared<VulkanQueue>(_device, VulkanQueue::EType::Graphic, _device->GetGraphicQueueFamilyIndex());
    TPtr<VulkanQueue> computeQueue = std::make_shared<VulkanQueue>(_device, VulkanQueue::EType::Compute, _device->GetComputeQueueFamilyIndex());
    TPtr<VulkanQueue> transferQueue = std::make_shared<VulkanQueue>(_device, VulkanQueue::EType::Transfer, _device->GetTransferQueueFamilyIndex()); 

    _queueArr = {graphicQueue, computeQueue, transferQueue};

    std::vector<VkDescriptorPoolSize> poolSizeArr = {{VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10}, {VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}};
    _descriptorPool = std::make_shared<VulkanDescriptorPool>(_device, poolSizeArr);

    _commandBufferManager = std::make_shared<VulkanCommandBufferManager>(_device, _queueArr);

    _bufferManager = std::make_shared<VulkanBufferManager>(_device);
}

void RenderSystem::Cleanup()
{
    {
        TPtr<RenderGraph> renderGraph = std::make_shared<RenderGraph>();
        std::unordered_set<RenderResource*>& renderResources = RenderResource::GetAll();
        for (RenderResource* resource : renderResources)
        {
            resource->CleanupRenderResource(renderGraph);
        }
    }

    _device->WaitIdle();

    _bufferManager.reset();
    _commandBufferManager.reset();
    _descriptorPool.reset();
    _queueArr.clear();

    _device.reset();
    _GPU.reset();

    DestroyVulkanInstance();
}

void RenderSystem::InitializeResources()
{
    TPtr<RenderGraph> renderGraph = std::make_shared<RenderGraph>();
    RenderResource::InitializeRenderResources(renderGraph);
}

void RenderSystem::CleanupResources()
{
    TPtr<RenderGraph> renderGraph = std::make_shared<RenderGraph>();
    RenderResource::CleanupRenderResources(renderGraph);
}

void RenderSystem::CreateVulkanInstance()
{
    const std::string appName{"ZEngine"};

    // Instance
    VkApplicationInfo vkAppInfo{};
    vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vkAppInfo.pApplicationName = appName.c_str();
    vkAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    vkAppInfo.pEngineName = appName.c_str();
    vkAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vkAppInfo.apiVersion = VK_HEADER_VERSION_COMPLETE;

    const std::vector<const char*> extensions = {
        "VK_KHR_surface",
#ifdef ZE_PLATFORM_WINDOWS
        "VK_KHR_win32_surface",
#endif
#ifdef ZE_PLATFORM_MACOS
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
#endif
    };

    const std::vector<const char*> validationLayers = {
#ifdef ZE_DEBUG
        "VK_LAYER_KHRONOS_validation"
#endif
    };

    VkInstanceCreateInfo vkInstanceCreateInfo{};
    vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkInstanceCreateInfo.pApplicationInfo = &vkAppInfo;
    vkInstanceCreateInfo.enabledExtensionCount = extensions.size();
    vkInstanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    vkInstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    vkInstanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#ifdef ZE_PLATFORM_MACOS
    vkInstanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    VkResult result = vkCreateInstance(&vkInstanceCreateInfo, nullptr, &_vkInstance);
    if (result != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vulkan instance!");
        return;
    }
}

void RenderSystem::DestroyVulkanInstance()
{
    if (_vkInstance != VK_NULL_HANDLE)
        vkDestroyInstance(_vkInstance, nullptr);
}

void RenderSystem::Tick()
{
}

TPtr<VulkanDevice> RenderSystem::GetDevice()
{
    return _device;
}

TPtr<VulkanBuffer> RenderSystem::AcquireBuffer(uint32_t size, VkBufferUsageFlags bits, VkMemoryPropertyFlags properties)
{
    return std::make_shared<VulkanBuffer>(_device, size, bits, properties);
}

TPtr<VulkanQueue> RenderSystem::GetQueue(VulkanQueue::EType type)
{
    size_t index = static_cast<size_t>(type) - 1;
    if (index >= _queueArr.size())
        return nullptr;
    else
        return _queueArr[index];
}

TPtr<VulkanDescriptorPool> RenderSystem::GetDescriptorPool()
{
    return _descriptorPool;
}

TPtr<VulkanCommandBufferManager> RenderSystem::GetCommandBufferManager()
{
    return _commandBufferManager;
}

TPtr<VulkanBufferManager> RenderSystem::GetBufferManager()
{
    return _bufferManager;
}

TPtrSet<VulkanGraphicPipeline>& RenderSystem::GetPipelineCache()
{
    return _pipelineCache;
}

} // namespace ZE