#include "RenderResource.h"


namespace ZE {

std::unordered_set<RenderResource*> RenderResource::_resourceSet;
std::unordered_set<RenderResource*> RenderResource::_initializedResourceSet;

std::unordered_set<RenderResource*>& RenderResource::GetAll()
{
    return _resourceSet;
}

std::unordered_set<RenderResource*>& RenderResource::GetInitialized()
{
    return _initializedResourceSet;
}

RenderResource::RenderResource()
{
    _resourceSet.insert(this);
}

RenderResource::~RenderResource()
{
    _resourceSet.erase(this);
}

void RenderResource::InitRenderResource(TPtr<RenderGraph> renderGraph)
{
    _initializedResourceSet.insert(this);
}

void RenderResource::CleanupRenderResource(TPtr<RenderGraph> renderGraph)
{
    _initializedResourceSet.erase(this);
}

bool RenderResource::IsRenderResourceInitialized()
{
    return _initializedResourceSet.contains(this);
}

}