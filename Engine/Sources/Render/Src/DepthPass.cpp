#include "DepthPass.h"
#include "Mesh.h"
#include "Material.h"
#include "RenderSystem.h"
#include "RenderTargets.h"
#include "Scene/SceneObject.h"
#include "Scene/MeshComponent.h"
#include "Resource/MaterialResource.h"
#include "Resource/MeshResource.h"
#include "Graphic/VulkanCommandBuffer.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanPipeline.h"
#include "Graphic/VulkanPipelineLayout.h"
#include "Graphic/VulkanRenderPass.h"
#include "Graphic/VulkanDescriptorSet.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanImageView.h"
#include "Graphic/VulkanBuffer.h"

namespace ZE {

void DepthPass::Setup(TPtr<VulkanImageView>& depth)
{
    m_depth = depth;
}

RenderTargets DepthPass::GetRenderTargets()
{
    RenderTargets renderTargets;
    renderTargets.depthStencil = RenderTargetBinding{m_depth, ERenderTargetLoadAction::Clear};
    
    return renderTargets;
}

void DepthPass::Draw(TPtrArr<SceneObject> objectsToRender, TPtr<VulkanCommandBuffer> commandBuffer)
{
    TPtr<VulkanDevice> device = commandBuffer->GetDevice();
    VkCommandBuffer vkCommandBuffer = commandBuffer->GetRawCommandBuffer();

    EPassType passType = EPassType::DepthPass;
    for (TPtr<SceneObject>& object : objectsToRender)
    {
        TPtr<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();

        TPtr<MeshResource> meshResource = meshComponent->GetMesh();
        TPtr<MaterialResource> materialResource = meshComponent->GetMaterial(0);

        TPtr<Mesh> mesh = meshResource->GetMesh();
        TPtr<Material> material = materialResource->GetMaterial();
        TPtr<Pass> pass = material->GetPass(passType);

        RHIPipelineState pipelineState;
        mesh->ApplyPipelineState(pipelineState);
        pass->ApplyPipelineState(pipelineState);
        TPtr<VulkanGraphicPipeline> pipeline = std::make_shared<VulkanGraphicPipeline>(device, pipelineState, _renderPass);
        RenderSystem::Get().GetPipelineCache().insert(pipeline);

        vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetRawPipeline());

        // Vertex Input
        VkBuffer vertexBuffers[] = {mesh->GetVertexBuffer()->GetRawBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(vkCommandBuffer, mesh->GetIndexBuffer()->GetRawBuffer(), 0, VK_INDEX_TYPE_UINT32);

        // Draw
        vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pass->GetPipelineLayout()->GetRawPipelineLayout(), 0, 1, &pass->GetDescriptorSet()->GetRawDescriptorSet(), 0, nullptr);
        vkCmdDrawIndexed(vkCommandBuffer, mesh->GetVerticesCount(), 1, 0, 0, 0);
    }
}

}