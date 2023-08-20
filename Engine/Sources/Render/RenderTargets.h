#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <optional>

namespace ZE {

class VulkanImageView;

enum class ERenderTargetLoadAction
{
    None = 0,
    Load,
    Clear,
};

struct RenderTargetBinding
{
    RenderTargetBinding(TPtr<VulkanImageView> inTarget, ERenderTargetLoadAction inAction)
        : target(inTarget), loadAction(inAction)
    {
    }

    TPtr<VulkanImageView> target;
    ERenderTargetLoadAction loadAction;
};

struct RenderTargets
{
    std::vector<RenderTargetBinding> colors;
    std::optional<RenderTargetBinding> depthStencil;
};

} // namespace ZE