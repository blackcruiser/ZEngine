#include "CoreDefines.h"

#include <vulkan/vulkan.h>


namespace TE {

class GPU;
class Window;

class Surface
{
public:
    Surface(VkInstance vkInstance, TPtr<GPU> GPU, TPtr<Window> window);
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

    TPtr<GPU> _GPU;
    TPtr<Window> _window;
};

}