#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <optional>

namespace ZE {

class VulkanImage;

enum class ERenderTargetLoadAction : uint8_t
{
    Load,
    Clear,
    DontCare,
};

struct RenderTargetBinding
{
    RenderTargetBinding(VulkanImage* inTarget, ERenderTargetLoadAction inAction)
        : target(inTarget), loadAction(inAction)
    {
    }

    VulkanImage* target;
    ERenderTargetLoadAction loadAction;
};

struct RenderTargets
{
    std::vector<RenderTargetBinding> colors;
    std::optional<RenderTargetBinding> depthStencil;
};

} // namespace ZE