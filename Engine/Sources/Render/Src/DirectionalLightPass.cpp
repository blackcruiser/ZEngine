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

void DirectionalLightPass::Draw(RenderGraph* renderGraph, SceneResource* sceneResource)
{

}

}