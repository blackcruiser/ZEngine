#include "Mesh.h"
#include "RenderSystem.h"
#include "Graphic/VulkanBuffer.h"
#include "Render/RenderingContext.h"
#include "Render/RenderGraph.h"
#include "Resource/MeshResource.h"


namespace ZE {

Mesh::Mesh(TPtr<MeshResource> meshResource)
    : _owner(meshResource), _vertexBuffer(nullptr), _indexBuffer(nullptr), _verticesCount(0)
{
}

Mesh::~Mesh()
{
}

uint32_t Mesh::GetVerticesCount()
{
    return _verticesCount;
}

void Mesh::CreateVertexBuffer(TPtr<RenderingContext> renderingContext, TPtr<RenderGraph> renderGraph)
{
    assert(_owner.expired() == false);

    TPtr<MeshResource> MeshResource = _owner.lock();

    const std::vector<VertexData>& vertices = MeshResource->GetVertices(0);
    uint32_t byteSize = static_cast<uint32_t>(vertices.size()) * sizeof(VertexData);

    renderGraph->CopyBuffer(reinterpret_cast<const uint8_t*>(vertices.data()), byteSize, _vertexBuffer);
}

TPtr<VulkanBuffer> Mesh::GetVertexBuffer()
{
    return _vertexBuffer;
}

void Mesh::CreateIndexBuffer(TPtr<RenderingContext> renderingContext, TPtr<RenderGraph> renderGraph)
{
    assert(_owner.expired() == false);

    TPtr<MeshResource> MeshResource = _owner.lock();

    const std::vector<uint32_t>& indexes = MeshResource->GetIndexes(0);
    uint32_t byteSize = static_cast<uint32_t>(indexes.size()) * sizeof(uint32_t);

    renderGraph->CopyBuffer(reinterpret_cast<const uint8_t*>(indexes.data()), byteSize, _indexBuffer);
    _verticesCount = static_cast<uint32_t>(indexes.size());
}

TPtr<VulkanBuffer> Mesh::GetIndexBuffer()
{
    return _indexBuffer;
}

void Mesh::ApplyPipelineState(RHIPipelineState& state)
{
    VkVertexInputBindingDescription& bindingDescription = state.vertexInputBinding;
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(VertexData);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription>& attributeDescriptions = state.vertexInputAttributes;
    attributeDescriptions.resize(3);
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

    VkPipelineVertexInputStateCreateInfo& vertexInputInfo = state.vertexInputState;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo& inputAssembly = state.inputAssemblyState;
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

}

} // namespace ZE
