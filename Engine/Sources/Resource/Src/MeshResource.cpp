#include "MeshResource.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace TE {

MeshResource::MeshResource(const std::filesystem::path& path) : _path(path), _mesh(nullptr)
{
}

void MeshResource::Load()
{
    std::string inputfile = "cornell_box.obj";

    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(_path.string(), reader_config))
    {
        if (!reader.Error().empty())
        {
            // std::cerr << "TinyObjReader: " << reader.Error();
        }
        return;
    }

    if (!reader.Warning().empty())
    {
        // std::cout << "TinyObjReader: " << reader.Warning();
    }

    const tinyobj::attrib_t& attrib = reader.GetAttrib();
    const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();

    for (const tinyobj::shape_t& shape : shapes)
    {
        std::vector<VertexData> verticesData(shape.mesh.indices.size());
        std::vector<uint32_t> indexesData(shape.mesh.indices.size());

        for (tinyobj::index_t index : shape.mesh.indices)
        {
            VertexData vertex{};

            vertex.position = {attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1],
                               attrib.vertices[3 * index.vertex_index + 2]};

            vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                               1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

            if (attrib.normals.empty() == false)
            {
                vertex.normal = {attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1],
                                 attrib.normals[3 * index.normal_index + 2]};
            }
            else
                vertex.normal = {1.0f, 1.0f, 1.0f};


            indexesData.push_back(verticesData.size());
            verticesData.push_back(vertex);
        }

        _meshVerticesData.push_back(verticesData);
        _meshIndexesData.push_back(indexesData);
    }

    _isLoaded = true;
}

void MeshResource::Unload()
{
}

uint32_t MeshResource::GetMeshCount()
{
    return _meshIndexesData.size();
}

const std::vector<VertexData>& MeshResource::GetVertices(uint32_t meshIndex)
{
    assert(meshIndex < _meshVerticesData.size());

    return _meshVerticesData[meshIndex];
}

const std::vector<uint32_t>& MeshResource::GetIndexes(uint32_t meshIndex)
{
    assert(meshIndex < _meshVerticesData.size());

    return _meshIndexesData[meshIndex];
}

void MeshResource::SetMesh(TPtr<Mesh> mesh)
{
    _mesh = mesh;
}

TPtr<Mesh> MeshResource::GetMesh()
{
    return _mesh;
}

} // namespace TE
