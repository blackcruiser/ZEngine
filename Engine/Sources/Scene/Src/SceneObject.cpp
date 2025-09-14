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

void SceneObject::PostLoad()
{
    TPtrArr<SceneComponent> components = GetComponents<SceneComponent>();
    for (TPtr<SceneComponent>& component : components)
    {
        component->PostLoad();
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
    ObjectResource* resource = new ObjectResource();

    TPtrArr<SceneComponent> components = GetComponents<SceneComponent>();
    for (TPtr<SceneComponent>& component : components)
    {
        ComponentResource* componentResource = component->CreateRenderResource();
        resource->AddComponentResource(componentResource);
    }

    return resource;
}

ObjectResource::ObjectResource()
{
}

void ObjectResource::AddComponentResource(ComponentResource* resource)
{
    _componentResources.push_back(resource);
}

}