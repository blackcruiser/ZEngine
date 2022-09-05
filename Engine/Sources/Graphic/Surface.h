#include "CoreDefines.h"

#include <vulkan/vulkan.h>


namespace TE {

class VulkanGPU;
class Window;

class Surface
{
public:
    Surface(VkInstance vkInstance, TPtr<VulkanGPU> GPU, TPtr<Window> window);
    ~Surface();

    VkSurfaceKHR GetRawSurface();
    VkSurfaceFormatKHR GetSurfaceFormat();
    VkPresentModeKHR GetPresentMode();
    VkExtent2D GetExtent();

    std::vector<VkSurfaceFormatKHR> GetSupportedSurfaceFormats();
    std::vector<VkPresentModeKHR> GetSupportedPresentModes();
    VkSurfaceCapabilitiesKHR GetCpabilities();

private:
    void _SelectSurfaceFormat();
    void _SelectPresentMode();
    void _CalculateExtent();

private:
    VkInstance _vkInstance;
    VkSurfaceKHR _vkSurface;
    VkSurfaceFormatKHR _vkSurfaceFormat;
    VkPresentModeKHR _presentMode;
    VkExtent2D _vkExtent;

    TPtr<VulkanGPU> _GPU;
    TPtr<Window> _window;
};

}