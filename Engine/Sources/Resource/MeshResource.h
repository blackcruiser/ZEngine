#pragma once

#include "BaseResource.h"
#include "CoreDefines.h"
#include "CoreTypes.h"

#include <glm/glm.hpp>

#include <filesystem>

namespace ZE {

class Mesh;

struct VertexData
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class MeshResource : BaseResource
{
public:
    MeshResource(const std::filesystem::path& path);

    virtual void Load() override;
    virtual void Unload() override;

    uint32_t GetMeshCount();

    const std::vector<VertexData>& GetVertices(uint32_t meshIndex);
    const std::vector<uint32_t>& GetIndexes(uint32_t meshIndex);

    void SetMesh(TPtr<Mesh> mesh);
    TPtr<Mesh> GetMesh();

private:
    std::filesystem::path _path;

    std::vector<std::vector<VertexData>> _meshVerticesData;
    std::vector<std::vector<uint32_t>> _meshIndexesData;

    TPtr<Mesh> _mesh;
};


} // namespace ZE