#include "RenderSystem.h"
#include "Graphic/VulkanGPU.h"
#include "Graphic/VulkanDevice.h"
#include "Render/RenderingContext.h"

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <cassert>


namespace ZE {

RenderSystem* RenderSystem::_instance{nullptr};

void RenderSystem::Initialize()
{
    assert(_instance == nullptr);

    if (_instance != nullptr)
        return;

    _instance = new RenderSystem();
}

void RenderSystem::Cleanup()
{
    assert(_instance);

    if (_instance == nullptr)
        return;


    delete _instance;
    _instance = nullptr;
}

RenderSystem& RenderSystem::Get()
{
    assert(_instance);

    return *_instance;
}

RenderSystem::RenderSystem()
    : _GPU(nullptr), _device(nullptr)
{
    _CreateVulkanInstance();

    _GPU = std::make_shared<VulkanGPU>(_vkInstance);
    _device = std::make_shared<VulkanDevice>(_GPU);


}

RenderSystem::~RenderSystem()
{
    _device->WaitIdle();

    _device.reset();
    _GPU.reset();

    _DestroyVulkanInstance();
}

void RenderSystem::_CreateVulkanInstance()
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

void RenderSystem::_DestroyVulkanInstance()
{
    if (_vkInstance != VK_NULL_HANDLE)
        vkDestroyInstance(_vkInstance, nullptr);
}

void RenderSystem::Tick()
{
}

TPtr<RenderingContext> RenderSystem::CreateRenderingContext()
{
    TPtr<RenderingContext> result = std::make_shared<RenderingContext>(_device);
    result->Initialize();

    return result;
}


TPtr<VulkanDevice> RenderSystem::GetDevice()
{
    return _device;
}

} // namespace ZE