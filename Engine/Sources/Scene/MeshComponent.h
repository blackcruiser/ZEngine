#pragma once

#include "CoreDefines.h"
#include "SceneComponent.h"

namespace TE {

class MeshResource;
class MaterialResource;

class MeshComponent : public SceneComponent
{
public:
    MeshComponent();
    virtual ~MeshComponent();

    virtual void Load() override;
    virtual void Unload() override;

    void SetMesh(TPtr<MeshResource> mesh);
    TPtr<MeshResource> GetMesh();

    void SetMaterial(uint32_t slot, TPtr<MaterialResource> material);
    TPtr<MaterialResource> GetMaterial(uint32_t slot);

private:
    TPtr<MeshResource> _mesh;
    TPtrArr<MaterialResource> _materialArr;
};

} // namespace TE
