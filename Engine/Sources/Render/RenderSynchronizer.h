#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>

#include <tuple>
#include <vector>


namespace ZE {

class RenderSynchronizer
{
public:
    RenderSynchronizer();
    ~RenderSynchronizer();

    VkFence GetFence();
    void MarkExecuted(VkFence fence, uint32 frameNumber);

private:
    std::vector<VkFence> m_freeFences;
    std::vector<std::tuple<VkFence, uint32>> m_pendingFences;
};

} // namespace ZE