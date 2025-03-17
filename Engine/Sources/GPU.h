#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.h>

class TESurface;
class TEDevice;

class TEGPU
{
public:
    TEGPU(const VkInstance &vkInstance);

    void Init();
    void Cleanup();

    VkPhysicalDevice GetRawPhysicalDevice();
    const std::vector<const char *> &GetExtensions();

    std::vector<VkExtensionProperties> GetExtensionProperties(VkPhysicalDevice GPU);
    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties();
    bool isSurfaceSupported(uint32_t queueFamilyIndex, TEPtr<TESurface> surface);

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    TEPtr<TEDevice> CreateDevice(TEPtr<TEGPU> GPU, TEPtr<TESurface> surface);

private:
    std::vector<VkPhysicalDevice> GetSupportedGPUs();

private:
    VkInstance _vkInstance;
    VkPhysicalDevice _GPU;

    const std::vector<const char *> _deviceExtensions;
};