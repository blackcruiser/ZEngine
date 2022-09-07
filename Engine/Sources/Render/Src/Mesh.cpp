#include "Mesh.h"
#include "Graphic/VulkanBuffer.h"
#include "Resource/MeshResource.h"


namespace TE {

Mesh::Mesh(TPtr<VulkanDevice> device, TPtr<MeshResource> meshResource)
    : _device(device), _owner(meshResource), _vertexBuffer(nullptr), _indexBuffer(nullptr), _verticesCount(0)
{
}

Mesh::~Mesh()
{
}

uint32_t Mesh::GetVerticesCount()
{
    return _verticesCount;
}

void Mesh::CreateVertexBuffer(TPtr<VulkanCommandPool> commandPool)
{
    assert(_owner.expired() == false);

    TPtr<MeshResource> MeshResource = _owner.lock();

    const std::vector<VertexData>& vertices = MeshResource->GetVertices(0);
    uint32_t byteSize = vertices.size() * sizeof(VertexData);

    _vertexBuffer = std::make_shared<VulkanBuffer>(_device, byteSize,
                                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    _vertexBuffer->TransferData(commandPool, vertices.data(), byteSize);
}

TPtr<VulkanBuffer> Mesh::GetVertexBuffer()
{
    return _vertexBuffer;
}

void Mesh::CreateIndexBuffer(TPtr<VulkanCommandPool> commandPool)
{
    assert(_owner.expired() == false);

    TPtr<MeshResource> MeshResource = _owner.lock();

    const std::vector<uint32_t>& indexes = MeshResource->GetIndexes(0);
    uint32_t byteSize = indexes.size() * sizeof(uint32_t);

    _indexBuffer = std::make_shared<VulkanBuffer>(_device, byteSize,
                                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    _indexBuffer->TransferData(commandPool, indexes.data(), byteSize);

    _verticesCount = indexes.size();
}

TPtr<VulkanBuffer> Mesh::GetIndexBuffer()
{
    return _indexBuffer;
}

VkVertexInputBindingDescription Mesh::GetVertexInputBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(VertexData);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> Mesh::GetVertexInputAttributeDescriptions()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(VertexData, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(VertexData, normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(VertexData, texCoord);

    return attributeDescriptions;
}

} // namespace TE
