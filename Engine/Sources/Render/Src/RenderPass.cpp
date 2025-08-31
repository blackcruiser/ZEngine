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

void RenderPass::Execute(RenderGraph*& renderGraph, const TPtrArr<SceneObject>& objectsToRender)
{
    Draw(renderGraph, objectsToRender);
}

} // namespace ZE