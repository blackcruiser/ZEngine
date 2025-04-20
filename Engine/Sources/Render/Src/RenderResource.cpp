#include "RenderResource.h"


namespace ZE {

std::unordered_set<RenderResource*> RenderResource::_resourceSet;
std::unordered_set<RenderResource*> RenderResource::_uninitializedResourceSet;

std::unordered_set<RenderResource*>& RenderResource::GetAll()
{
    return _resourceSet;
}

std::unordered_set<RenderResource*>& RenderResource::GetUninitialized()
{
    return _uninitializedResourceSet;
}

void RenderResource::InitializeRenderResources(TPtr<RenderGraph> renderGraph)
{
    for (auto iter = _uninitializedResourceSet.begin(); iter != _uninitializedResourceSet.end();)
    {
        RenderResource* resource = *iter;
        resource->InitRenderResource(renderGraph);
        iter = _uninitializedResourceSet.erase(iter);
    }
}

void RenderResource::CleanupRenderResources(TPtr<RenderGraph> renderGraph)
{
    for (auto iter = _resourceSet.begin(); iter != _resourceSet.end(); iter++)
    {
        RenderResource* resource = *iter;
        resource->CleanupRenderResource(renderGraph);
         _uninitializedResourceSet.insert(resource);
    }
}

RenderResource::RenderResource()
{
    _resourceSet.insert(this);
    _uninitializedResourceSet.insert(this);
}

RenderResource::~RenderResource()
{
    _resourceSet.erase(this);
    _uninitializedResourceSet.erase(this);
}

void RenderResource::InitRenderResource(TPtr<RenderGraph> renderGraph)
{
}

void RenderResource::CleanupRenderResource(TPtr<RenderGraph> renderGraph)
{
}

bool RenderResource::IsRenderResourceInitialized()
{
    return !_uninitializedResourceSet.contains(this);
}

} // namespace ZE