#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"


namespace ZE {

class Scene;
class Frame;
class VulkanCommandBuffer;


class RendererInterface
{
public:
    virtual void Init(TPtr<Scene> scene) = 0;
    virtual void RenderFrame(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<Scene> scene, TPtr<Frame> frame) = 0;
};

} // namespace ZE