#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>

#include <memory>


namespace TE {

class GPU;
class Device;
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
    TPtr<GPU> _GPU;
    TPtr<Device> _device;
    TPtr<Window> _window;
    TPtr<Surface> _surface;
    TPtr<RendererInterface> _renderer;

    VkInstance _vkInstance;
};

}