#pragma once

#include "CoreDefines.h"
#include "SceneComponent.h"

#include <glm/glm.hpp>


namespace TE {

class CameraComponent : public SceneComponent
{
public:
    CameraComponent();
    virtual ~CameraComponent();

    glm::mat4x4 GetViewMatrix();

    void SetProjectMatrix(const glm::mat4x4& matrix);
    const glm::mat4x4& GetProjectMatrix();

private:
    glm::mat4x4 _projectMatrix;
};

}