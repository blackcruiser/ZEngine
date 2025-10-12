#include "DepthPass.h"
#include "Mesh.h"
#include "Material.h"
#include "Graphic/PipelineState.h"
#include "RenderSystem.h"
#include "RenderGraph.h"
#include "RenderTargets.h"
#include "Scene/SceneObject.h"
#include "Scene/MeshComponent.h"
#include "Resource/MaterialResource.h"
#include "Resource/MeshResource.h"


namespace ZE {

void DepthPass::Init(TPtr<VulkanImage> depthRenderTarget)
{
    renderTargets = std::make_shared<RenderTargets>();
    renderTargets->depthStencil = RenderTargetBinding{depthRenderTarget, ERenderTargetLoadAction::Clear};
}

void DepthPass::Draw(RenderGraph* renderGraph, SceneResource* sceneResource)
{
}

}