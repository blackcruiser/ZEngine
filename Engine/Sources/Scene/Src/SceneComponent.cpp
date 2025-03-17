#include "SceneComponent.h"


namespace ZE {

SceneComponent::SceneComponent(const EComponentType type) : _type(type)
{
}

SceneComponent::~SceneComponent()
{
}

const EComponentType SceneComponent::GetType() const
{
    return _type;
}

void SceneComponent::SetObject(TPtr<SceneObject> object)
{
    _sceneObject = object;
}

TPtr<SceneObject> SceneComponent::GetObject()
{
    return _sceneObject.lock();
}

}