#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <optional>

namespace ZE {

class VulkanCommandBuffer;
class VulkanImageView;
class VulkanRenderPass;
class SceneObject;
class Scene;
class Frame;
struct RenderTargets;

class RenderPass
{
public:
    RenderPass();
    ~RenderPass();

    virtual void Prepare(TPtr<Scene> scene) = 0;
    void Execute(TPtrArr<SceneObject> objectsToRender, TPtr<VulkanCommandBuffer> commandBuffer, TPtr<Frame> frame, TPtr<RenderTargets> renderTargets);
    virtual void Draw(TPtrArr<SceneObject> objectsToRender, TPtr<VulkanCommandBuffer> commandBuffer, TPtr<RenderTargets> renderTargets) = 0;

protected:
    TPtr<VulkanRenderPass> _renderPass;
};

}