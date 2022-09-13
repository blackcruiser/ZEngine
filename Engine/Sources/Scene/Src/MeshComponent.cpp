#include "MeshComponent.h"
#include "Resource/MaterialResource.h"
#include "Resource/MeshResource.h"


namespace ZE {

MeshComponent::MeshComponent() : SceneComponent(EComponentType::Mesh), _mesh(nullptr)
{
}

MeshComponent::~MeshComponent()
{
}

void MeshComponent::Load()
{
    SceneComponent::Load();

    if (_mesh != nullptr)
        _mesh->Load();

    for (const TPtr<MaterialResource>& material : _materialArr)
    {
        material->Load();
    }
}

void MeshComponent::Unload()
{
    if (_mesh != nullptr)
        _mesh->Unload();

    for (const TPtr<MaterialResource>& material : _materialArr)
    {
        material->Unload();
    }
    SceneComponent::Unload();
}

void MeshComponent::SetMesh(TPtr<MeshResource> mesh)
{
    _mesh = mesh;
}

TPtr<MeshResource> MeshComponent::GetMesh()
{
    return _mesh;
}

void MeshComponent::SetMaterial(uint32_t slot, TPtr<MaterialResource> material)
{
    if (slot >= _materialArr.size())
        _materialArr.resize(slot + 1);

    _materialArr[slot] = material;
}

TPtr<MaterialResource> MeshComponent::GetMaterial(uint32_t slot)
{
    if (slot < _materialArr.size())
        return _materialArr[slot];
    else
        return nullptr;
}

} // namespace ZE