#pragma once

#include "CoreDefines.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


namespace TE {

class VulkanGPU;
class Surface;

class VulkanDevice
{
public:
    VulkanDevice(TPtr<VulkanGPU> GPU, TPtr<Surface> surface);
    ~VulkanDevice();

    VkSemaphore CreateGraphicSemaphore();
    void DestroyGraphicSemaphore(VkSemaphore semaphore);

    VkFence CreateFence(bool isSignaled);
    void DestroyFence(VkFence fence);

    VkImageView CreateImageView(VkImage image, VkFormat format);
    void DestroyImageView(VkImageView imageView);

    VkSampler CreateTextureSampler();
    void DestroyTextureSampler(VkSampler sampler);

    VkFramebuffer CreateFramebuffer(VkRenderPass renderPass, const std::vector<VkImageView> imageViewArr,
                                    uint32_t width, uint32_t height);
    void DestroyFramebuffer(VkFramebuffer framebuffer);

    VkSwapchainKHR CreateSwapchain(uint32_t imageCount, VkFormat imageFormat, VkColorSpaceKHR colorSpace,
                                   VkExtent2D extent, VkSurfaceTransformFlagBitsKHR preTransform,
                                   VkPresentModeKHR presentMode);
    void DestroySwapchain(VkSwapchainKHR swapchain);

    VkPipelineLayout CreatePipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
    void DestroyPipelineLayout(VkPipelineLayout pipelineLayout);

    VkRenderPass CreateRenderPass(VkFormat format);
    void DestroyRenderPass(VkRenderPass renderPass);

    VkDescriptorSetLayout CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings);
    void DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);


    void WaitIdle();

    TPtr<VulkanGPU> GetGPU();

    VkDevice GetRawDevice();
    VkQueue GetGraphicQueue();
    VkQueue GetPresentQueue();

    uint32_t GetGraphicQueueFamilyIndex();
    uint32_t GetPresentQueueFamilyIndex();

private:
    VkDevice _vkDevice;
    VkQueue _vkGraphicQueue, _vkPresentQueue;
    uint32_t _graphicQueueFamilyIndex, _presentQueueFamilyIndex;

    TPtr<VulkanGPU> _GPU;
    TPtr<Surface> _surface;
};

} // namespace TE