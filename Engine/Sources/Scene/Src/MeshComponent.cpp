#include "MeshComponent.h"
#include "Resource/MaterialResource.h"
#include "Resource/MeshResource.h"
#include "Render/Mesh.h"


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

void MeshComponent::PostLoad()
{
    SceneComponent::PostLoad();

    if (_mesh != nullptr)
        _mesh->PostLoad();

    for (const TPtr<MaterialResource>& material : _materialArr)
    {
        material->PostLoad();
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

ComponentResource* MeshComponent::CreateRenderResource()
{
    Mesh* mesh = _mesh->GetMesh();

    std::vector<Material*> materials;
    std::transform(_materialArr.begin(), _materialArr.end(), std::back_inserter(materials), [](TPtr<MaterialResource> material) {
        return material->GetMaterial();
    });

    MeshComponentResource* resource = new MeshComponentResource(mesh, materials);

    return resource;
}


MeshComponentResource::MeshComponentResource(Mesh* inMesh, std::vector<Material*>& inMaterials) :
    _mesh(inMesh), _materials(inMaterials)
{
}

MeshComponentResource::~MeshComponentResource()
{
}

void MeshComponentResource::InitGraphic(RenderGraph* renderGraph)
{
}

void MeshComponentResource::CleanupGraphic()
{
}

} // namespace ZE