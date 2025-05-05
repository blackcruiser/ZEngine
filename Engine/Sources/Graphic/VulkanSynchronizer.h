#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;

VkSemaphore CreateSemaphore(VulkanDevice* device);
void DestroySemaphore(VulkanDevice* device, VkSemaphore semaphore);

VkFence CreateFence(VulkanDevice* device, bool bSignaled);
void ResetFence(VulkanDevice* device, VkFence fence);
void DestroyFence(VulkanDevice* device, VkFence fence);

bool IsSignaled(VulkanDevice* device, VkFence fence);

}