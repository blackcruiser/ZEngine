#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "SceneComponent.h"

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
};

}