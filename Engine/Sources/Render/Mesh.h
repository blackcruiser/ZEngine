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
    Mesh(MeshResource* meshResource);
    ~Mesh();

    virtual void InitGraphic(RenderGraph* renderGraph) override;
    virtual void CleanupGraphic() override;

    uint32_t GetVerticesCount();
    TPtr<VulkanBuffer> GetVertexBuffer();
    TPtr<VulkanBuffer> GetIndexBuffer();

    void ApplyPipelineState(RHIPipelineState& state);

private:
    TPtr<VulkanBuffer> _vertexBuffer;
    TPtr<VulkanBuffer> _indexBuffer;
    uint32_t _verticesCount;

    MeshResource* _owner;

public:
    VkVertexInputBindingDescription vertexInputBindingDescription;
    std::vector<VkVertexInputAttributeDescription> VertexInputAttributeDescriptions;
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo;
};

} // namespace ZE
