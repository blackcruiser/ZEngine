#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.h>

#include <memory>


namespace TE {

class VulkanGPU;
class VulkanDevice;
class Window;
class Surface;
class RendererInterface;

class Scene;

class Application
{
public:
    Application();
    ~Application();

    void Run(TPtr<Scene> scene);

private:
    void _InitGlfw();
    void _CreateVulkanInstance();

    void _CleanupGlfw();
    void _CleanupVulkan();

private:
    TPtr<VulkanGPU> _GPU;
    TPtr<VulkanDevice> _device;
    TPtr<Window> _window;
    TPtr<Surface> _surface;
    TPtr<RendererInterface> _renderer;

    VkInstance _vkInstance;
};

}