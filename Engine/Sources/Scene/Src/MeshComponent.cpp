#include "MeshComponent.h"


namespace TE {

MeshComponent::MeshComponent() : SceneComponent(EComponentType::Mesh)
{
}

MeshComponent::~MeshComponent()
{
}

void MeshComponent::SetVertices(const std::vector<Vertex>& vertices)
{
    _vertices = vertices;
}

const std::vector<Vertex>& MeshComponent::GetVertices()
{
    return _vertices;
}

void MeshComponent::SetIndexes(const std::vector<uint32_t>& indexes)
{
    _indexes = indexes;
}

const std::vector<uint32_t>& MeshComponent::GetIndexes()
{
    return _indexes;
}

}