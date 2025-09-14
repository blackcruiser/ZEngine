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

    virtual ComponentResource* CreateRenderResource() override;

private:
    TPtr<MeshResource> _mesh;
    TPtrArr<MaterialResource> _materialArr;
};

class MeshComponentResource : public ComponentResource
{
public:
    MeshComponentResource();
    ~MeshComponentResource();

    virtual void Init(RenderGraph* renderGraph) override;
    virtual void Cleanup() override;

    virtual void InitGraphic(RenderGraph* renderGraph) override;
    virtual void CleanupGraphic() override;

private:
    Mesh* _mesh;
    Material* _material;
}

} // namespace ZE
