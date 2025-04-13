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

void RenderPass::Execute(TPtr<RenderingContext>& renderingContext, TPtr<RenderGraph>& renderGraph, const TPtrArr<SceneObject>& objectsToRender)
{
    renderGraph->BeginRenderPass();

    Draw(renderingContext, renderGraph, objectsToRender);
    
    renderGraph->EndRenderPass();
}

} // namespace ZE