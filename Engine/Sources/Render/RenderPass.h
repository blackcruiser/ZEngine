#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <glm/vec2.hpp>
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

    virtual RenderTargets GetRenderTargets();

    virtual void Execute(TPtrArr<SceneObject> objectsToRender, TPtr<VulkanCommandBuffer> commandBuffer, const glm::ivec2& viewport);

    virtual void Draw(TPtrArr<SceneObject> objectsToRender, TPtr<VulkanCommandBuffer> commandBuffer) = 0;

protected:
 
};

}