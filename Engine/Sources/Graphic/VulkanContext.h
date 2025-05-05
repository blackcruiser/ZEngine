#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

VkInstance CreateInstance();

void DestroyInstance(VkInstance instance);

const std::vector<const char*>& GetExtensions();

std::vector<VkPhysicalDevice> GetPhysicalDevices(VkInstance instance);

std::vector<VkExtensionProperties> GetExtensionProperties(VkPhysicalDevice physicalDevice);

VkPhysicalDevice AcquirePhysicalDevice(VkInstance instance);

std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties(VkPhysicalDevice physicalDevice);

VkPhysicalDeviceMemoryProperties GetMemoryProperties(VkPhysicalDevice physicalDevice);

uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

}