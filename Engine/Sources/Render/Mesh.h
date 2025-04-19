#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderResource.h"

#include "Graphic/VulkanPipeline.h"

#include <vulkan/vulkan.h>


namespace ZE {

class MeshResource;
class VulkanBuffer;
class RenderGraph;

class Mesh : public RenderResource
{
public:
    Mesh(TPtr<MeshResource> meshResource);
    ~Mesh();

    virtual void InitRenderResource(TPtr<RenderGraph> renderGraph) override;
    virtual void CleanupRenderResource(TPtr<RenderGraph> renderGraph) override;

    uint32_t GetVerticesCount();
    TPtr<VulkanBuffer> GetVertexBuffer();
    TPtr<VulkanBuffer> GetIndexBuffer();

    void ApplyPipelineState(RHIPipelineState& state);

private:
    TPtr<VulkanBuffer> _vertexBuffer, _indexBuffer;
    uint32_t _verticesCount;

    TWeakPtr<MeshResource> _owner;
};

} // namespace ZE
