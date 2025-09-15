#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "SceneComponent.h"
#include "Graphic/VulkanBuffer.h"

#include <glm/glm.hpp>


namespace ZE {

class TransformComponent : public SceneComponent
{
public:
    TransformComponent();
    virtual ~TransformComponent();

    void SetTransform(const glm::mat4x4& transform);
    const glm::mat4x4& GetTransform();

private:
    glm::mat4x4 _transform;

public:
    virtual ComponentResource* CreateRenderResource() override;

private:
    ComponentResource* componentResource;
};


class TransformComponentResource : public ComponentResource
{
public:
    TransformComponentResource();
    ~TransformComponentResource();

    virtual void Init(RenderGraph* renderGraph) override;
    virtual void Cleanup() override;

    virtual void InitGraphic(RenderGraph* renderGraph) override;
    virtual void CleanupGraphic() override;

private:
    TPtr<VulkanBuffer> _MVPBuffer;
};

}