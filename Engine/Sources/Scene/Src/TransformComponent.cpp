#include "TransformComponent.h"


namespace TE {

TransformComponent::TransformComponent() : SceneComponent(EComponentType::Transform), _transform(1)
{
}

TransformComponent::~TransformComponent()
{
}

void TransformComponent::SetTransform(const glm::mat4x4& transform)
{
    _transform = transform;
}

const glm::mat4x4& TransformComponent::GetTransform()
{
    return _transform;
}

}