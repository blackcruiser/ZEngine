#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"
#include "GraphicResource.h"


namespace ZE {

class VulkanSampler : public GraphicResource, public VulkanDeviceChild
{
public:
    VulkanSampler(VulkanDevice* device);
    ~VulkanSampler();

    VkSampler GetRawSampler();

private:
    VkSampler _sampler;
};

} // namespace ZE
