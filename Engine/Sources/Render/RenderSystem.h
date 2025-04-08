#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "Graphic/VulkanQueue.h"

namespace ZE {

class VulkanGPU;
class VulkanDevice;
class RenderingContext;

class RenderSystem
{
public:
    static void Initialize();
    static void Cleanup();
    static RenderSystem& Get();

private:
    RenderSystem();
    ~RenderSystem();

    void _CreateVulkanInstance();
    void _DestroyVulkanInstance();

public:
    void Tick();

    TPtr<VulkanDevice> GetDevice();

    TPtr<RenderingContext> CreateRenderingContext();

private:
    static RenderSystem* _instance;
    VkInstance _vkInstance;

    TPtr<VulkanGPU> _GPU;
    TPtr<VulkanDevice> _device;
};

} // namespace ZE