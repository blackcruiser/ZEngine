#include "RenderPass.h"
#include "RenderTargets.h"


namespace ZE {

RenderPass::RenderPass()
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::Execute(TPtr<RenderingContext>& renderingContext, TPtr<RenderGraph>& commandBuffer, const TPtrArr<SceneObject>& objectsToRender)
{
}

} // namespace ZE