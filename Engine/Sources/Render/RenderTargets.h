#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <glm/vec4.hpp>

#include <optional>

namespace ZE {

class VulkanImageView;

enum class ERenderTargetLoadAction : uint8_t
{
    Load,
    Clear,
    DontCare,
};

struct RenderTargetBinding
{
    RenderTargetBinding(TPtr<VulkanImageView> inTarget, ERenderTargetLoadAction inAction, glm::vec4 inClearValue)
        : target(inTarget), loadAction(inAction), clearValue(inClearValue)
    {
    }

    TPtr<VulkanImageView> target;
    ERenderTargetLoadAction loadAction;
    glm::vec4 clearValue;
};

struct RenderTargets
{
    std::vector<RenderTargetBinding> colors;
    std::optional<RenderTargetBinding> depthStencil;
};

} // namespace ZE