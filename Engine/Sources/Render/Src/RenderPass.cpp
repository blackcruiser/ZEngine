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

void RenderPass::Execute(TPtr<RenderGraph>& renderGraph, const TPtrArr<SceneObject>& objectsToRender)
{
    renderGraph->BeginRenderPass();

    Draw(renderGraph, objectsToRender);
    
    renderGraph->EndRenderPass();
}

} // namespace ZE