#include "RenderPass.h"
#include "RenderTargets.h"
#include "RenderGraph.h"


namespace ZE {

RenderPass::RenderPass()
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::Execute(RenderGraph* renderGraph, SceneResource* sceneResource)
{
    renderGraph->SetRenderTargets(renderTargets);
    renderGraph->BeginRenderPass();

    renderGraph->SetPassParameter();

    Draw(renderGraph, sceneResource);

    renderGraph->EndRenderPass();
}

} // namespace ZE