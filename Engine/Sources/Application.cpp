#include "Application.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanGPU.h"
#include "Graphic/Surface.h"
#include "Graphic/Window.h"
#include "Input/InputSystem.h"
#include "Render/ForwardRenderer.h"
#include "Scene/Scene.h"

#include <set>
#include <stdexcept>
#include <string>


namespace TE {

const std::string AppName("ToyEngine");
const int kWidth = 800;
const int kHeight = 800;

Application::Application() : _device(nullptr), _window(nullptr), _renderer(nullptr)
{

    _InitGlfw();
    _CreateVulkanInstance();

    _window = std::make_shared<Window>(AppName, kWidth, kHeight);
    _window->RegisterInput(InputSystem::GetInstance());

    _GPU = std::make_shared<VulkanGPU>(_vkInstance);
    _surface = std::make_shared<Surface>(_vkInstance, _GPU, _window);
    _device = std::make_shared<VulkanDevice>(_GPU, _surface);
    _renderer = std::make_shared<ForwardRenderer>(_device, _surface);
}

Application::~Application()
{
    _renderer.reset();
    _device.reset();
    _surface.reset();
    _GPU.reset();

    _window->UnregisterInput(InputSystem::GetInstance());
    _window.reset();

    _CleanupVulkan();
    _CleanupGlfw();
}

void Application::_InitGlfw()
{
    glfwInit();
}

void Application::_CreateVulkanInstance()
{
    // Instance
    VkApplicationInfo vkAppInfo{};
    vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vkAppInfo.pApplicationName = AppName.c_str();
    vkAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    vkAppInfo.pEngineName = "No Engine";
    vkAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vkAppInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    const std::vector<const char*> validationLayers = {
#ifdef TOYENGINE_DEBUG
        "VK_LAYER_KHRONOS_validation"
#endif
    };

    VkInstanceCreateInfo vkInstanceCreateInfo{};
    vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkInstanceCreateInfo.pApplicationInfo = &vkAppInfo;
    vkInstanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
    vkInstanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    vkInstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    vkInstanceCreateInfo.ppEnabledLayerNames = validationLayers.data();

    if (vkCreateInstance(&vkInstanceCreateInfo, nullptr, &_vkInstance) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vulkan instance!");
        return;
    }
}

void Application::Run(TPtr<Scene> scene)
{
    scene->Load();

    _renderer->Init(scene);

    while (!_window->ShouldClose())
    {
        glfwPollEvents();

        _renderer->RenderFrame(scene);
    }

    _device->WaitIdle();

    scene->Unload();
}

void Application::_CleanupVulkan()
{

    if (_vkInstance != VK_NULL_HANDLE)
        vkDestroyInstance(_vkInstance, nullptr);
}

void Application::_CleanupGlfw()
{
    glfwTerminate();
}

} // namespace TE