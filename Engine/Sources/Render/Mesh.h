#pragma once

#include "ZEDefine.h"
#include "ZEType.h"

#include "Graphic/VulkanPipeline.h"

#include <vulkan/vulkan.h>


namespace ZE {

class MeshResource;

class VulkanBuffer;
class VulkanCommandBuffer;
class VulkanDevice;


class Mesh
{
public:
    Mesh(TPtr<MeshResource> meshResource);
    ~Mesh();

    uint32_t GetVerticesCount();

    void CreateVertexBuffer(TPtr<VulkanCommandBuffer> commandBuffer);
    TPtr<VulkanBuffer> GetVertexBuffer();

    void CreateIndexBuffer(TPtr<VulkanCommandBuffer> commandBuffer);
    TPtr<VulkanBuffer> GetIndexBuffer();

    void BuildPipelineDesc(VulkanGraphicPipelineDesc& desc);

private:
    TPtr<VulkanBuffer> _vertexBuffer, _indexBuffer;
    uint32_t _verticesCount;

    TWeakPtr<MeshResource> _owner;
};

} // namespace ZE
