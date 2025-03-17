#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


namespace TE {

class GPU;
class Surface;

class Device
{
public:
    Device(TPtr<GPU> GPU, TPtr<Surface> surface);
    ~Device();

    VkBuffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage);
    void DestroyBuffer(VkBuffer buffer);

    VkDeviceMemory AllocateAndBindBufferMemory(VkBuffer buffer, VkMemoryPropertyFlags properties);
    void FreeMemory(VkDeviceMemory deviceMemory);

    VkSemaphore CreateGraphicSemaphore();
    void DestroyGraphicSemaphore(VkSemaphore semaphore);

    VkFence CreateFence(bool isSignaled);
    void DestroyFence(VkFence fence);

    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    void DestroyShaderModule(VkShaderModule shaderModule);

    VkImage CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiliing,
                        VkImageUsageFlags usage);
    void DestroyImage(VkImage image);

    VkDeviceMemory AllocateAndBindImageMemory(VkImage image, VkMemoryPropertyFlags properties);

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

    VkPipeline CreateGraphicPipeline(VkShaderModule vertexShaderModule, VkShaderModule fragmentShaderModule,
                                     VkExtent2D extent, const VkVertexInputBindingDescription& bindingDescription,
                                     std::vector<VkVertexInputAttributeDescription> attributeDescriptions,
                                     VkPipelineLayout pipelineLayout, VkRenderPass renderPass);
    void DestroyPipeline(VkPipeline pipeline);

    VkRenderPass CreateRenderPass(VkFormat format);
    void DestroyRenderPass(VkRenderPass renderPass);

    VkDescriptorSetLayout CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings);
    void DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);

    VkDescriptorPool CreateDescriptorPool(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizeArr);
    void DestroyDescriptorPool(VkDescriptorPool descriptorPool);

    VkDescriptorSet AllocateDescriptorSet(VkDescriptorPool descriptorPool, uint32_t descriptorSetCount,
                                          const VkDescriptorSetLayout* pSetLayouts);
    void FreeDescriptorSet(VkDescriptorPool descriptorPool, uint32_t descriptorSetCount,
                           const VkDescriptorSet* pSetLayouts);
    void UpdateDescriptorSet(const std::vector<VkWriteDescriptorSet>& descriptorWrites);

    void WaitIdle();

    VkDevice GetRawDevice();

    VkQueue GetGraphicQueue();
    VkQueue GetPresentQueue();

    uint32_t GetGraphicQueueFamilyIndex();
    uint32_t GetPresentQueueFamilyIndex();

private:
    VkDevice _vkDevice;
    VkQueue _vkGraphicQueue, _vkPresentQueue;
    uint32_t _graphicQueueFamilyIndex, _presentQueueFamilyIndex;

    TPtr<GPU> _GPU;
    TPtr<Surface> _surface;
};

} // namespace TE