#include "SceneObject.h"


namespace ZE {

SceneObject::SceneObject()
{
}

SceneObject::~SceneObject()
{
    _componentArr.clear();
}

void SceneObject::Load()
{
    TPtrArr<SceneComponent> components = GetComponents<SceneComponent>();
    for (TPtr<SceneComponent>& component : components)
    {
        component->Load();
    }
}

void SceneObject::Unload()
{
    TPtrArr<SceneComponent> components = GetComponents<SceneComponent>();
    for (TPtr<SceneComponent>& component : components)
    {
        component->Unload();
    }
}

ObjectResource* SceneObject::CreateRenderResource()
{
    TPtrArr<SceneComponent> components = GetComponents<SceneComponent>();
    for (TPtr<SceneComponent>& component : components)
    {
        component->CreateRenderResource();
    }
}

}