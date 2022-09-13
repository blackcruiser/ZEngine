#include "CameraComponent.h"
#include "TransformComponent.h"
#include "SceneObject.h"

#include <glm/ext.hpp>


namespace ZE {

CameraComponent::CameraComponent() : SceneComponent(EComponentType::Camera), _projectMatrix(glm::identity<glm::mat4x4>())
{
}

CameraComponent::~CameraComponent()
{
}

glm::mat4x4 CameraComponent::GetViewMatrix()
{
    glm::mat4x4 viewMatrix{ glm::identity<glm::mat4x4>() };

    TPtr<TransformComponent> transformComponent = GetObject() ? GetObject()->GetComponent<TransformComponent>() : nullptr;
    if (transformComponent != nullptr)
    {
        viewMatrix = glm::inverse(transformComponent->GetTransform());
    }

    return viewMatrix;
}

void CameraComponent::SetProjectMatrix(const glm::mat4x4& matrix)
{
    _projectMatrix = matrix;
}

const glm::mat4x4& CameraComponent::GetProjectMatrix()
{
    return _projectMatrix;
}

}