#include "DirectionalLightPass.h"
#include "Mesh.h"
#include "Material.h"
#include "Graphic/PipelineState.h"
#include "RenderSystem.h"
#include "Render/RenderGraph.h"
#include "RenderTargets.h"
#include "Scene/SceneObject.h"
#include "Scene/MeshComponent.h"
#include "Resource/MaterialResource.h"
#include "Resource/MeshResource.h"


namespace ZE {

void DirectionalLightPass::Init(TPtr<VulkanImage> colorRenderTarget, TPtr<VulkanImage> depthRenderTarget)
{
    renderTargets = std::make_shared<RenderTargets>();
    renderTargets->colors = {RenderTargetBinding{colorRenderTarget, ERenderTargetLoadAction::Clear}};
    renderTargets->depthStencil = RenderTargetBinding{depthRenderTarget, ERenderTargetLoadAction::Load};
}

void DirectionalLightPass::Draw(RenderGraph*& renderGraph, const TPtrArr<SceneObject>& objectsToRender)
{
    renderGraph->SetRenderTargets(renderTargets);
    renderGraph->BeginRenderPass();

    EPassType passType = EPassType::BasePass;
    for (const TPtr<SceneObject>& object : objectsToRender)
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
        renderGraph->SetPipelineState(pipelineState, pass->GetDescriptorSet());

        renderGraph->BindVertexBuffer(mesh->GetVertexBuffer(), mesh->GetIndexBuffer());

        // Draw
        renderGraph->DrawIndexed(mesh->GetVerticesCount(), 0);
    }

    renderGraph->EndRenderPass();
}

}