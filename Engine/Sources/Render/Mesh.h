#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>


namespace TE {

class MeshResource;

class VulkanBuffer;
class VulkanCommandPool;
class VulkanDevice;


class Mesh
{
public:
    Mesh(TPtr<VulkanDevice> device, TPtr<MeshResource> meshResource);
    ~Mesh();

    uint32_t GetVerticesCount();

    void CreateVertexBuffer(TPtr<VulkanCommandPool> commandPool);
    TPtr<VulkanBuffer> GetVertexBuffer();

    void CreateIndexBuffer(TPtr<VulkanCommandPool> commandPool);
    TPtr<VulkanBuffer> GetIndexBuffer();

    VkVertexInputBindingDescription GetVertexInputBindingDescription();
    std::vector<VkVertexInputAttributeDescription> GetVertexInputAttributeDescriptions();

private:
    TPtr<VulkanBuffer> _vertexBuffer, _indexBuffer;
    uint32_t _verticesCount;

    TWeakPtr<MeshResource> _owner;
    TPtr<VulkanDevice> _device;
};

} // namespace TE
