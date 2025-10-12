#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "SceneComponent.h"

namespace ZE {

class MeshResource;
class MaterialResource;
class Mesh;
class Material;

class MeshComponent : public SceneComponent
{
public:
    MeshComponent();
    virtual ~MeshComponent();

    virtual void Load() override;
    virtual void PostLoad() override;

    virtual void Unload() override;

    void SetMesh(TPtr<MeshResource> mesh);
    TPtr<MeshResource> GetMesh();

    void SetMaterial(uint32_t slot, TPtr<MaterialResource> material);
    TPtr<MaterialResource> GetMaterial(uint32_t slot);

private:
    TPtr<MeshResource> _mesh;
    TPtrArr<MaterialResource> _materialArr;

public:
    virtual ComponentResource* CreateRenderResource() override;

private:
    ComponentResource* componentResource;
};

class MeshComponentResource : public ComponentResource
{
public:
    MeshComponentResource(Mesh* inMesh, std::vector<Material*>& inMaterials);
    ~MeshComponentResource();

    virtual void InitGraphic(RenderGraph* renderGraph) override;
    virtual void CleanupGraphic() override;

private:
    Mesh* _mesh;
    std::vector<Material*> _materials;
};

} // namespace ZE
