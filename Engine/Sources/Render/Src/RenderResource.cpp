#include "RenderResource.h"
#include "RenderSystem.h"


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

void RenderResource::InitializeRenderResources(RenderGraph* renderGraph)
{
    for (auto iter = _uninitializedResourceSet.begin(); iter != _uninitializedResourceSet.end();)
    {
        RenderResource* resource = *iter;
        resource->InitGraphic(renderGraph);
        iter = _uninitializedResourceSet.erase(iter);
    }
}

void RenderResource::CleanupGraphics(RenderGraph* renderGraph)
{
    for (auto iter = _resourceSet.begin(); iter != _resourceSet.end(); iter++)
    {
        RenderResource* resource = *iter;
        resource->CleanupGraphic(renderGraph);
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

void RenderResource::Init()
{
    // ToDo: Run in RenderThread.
    RenderGraph* renderGraph = RenderSystem::Get().GetRenderGraph();
    InitGraphic(renderGraph);
}

void RenderResource::Cleanup()
{
    // ToDo: Run in RenderThread.
    CleanupGraphic();
}

void RenderResource::InitGraphic(RenderGraph* renderGraph)
{
}

void RenderResource::CleanupGraphic()
{
}

bool RenderResource::IsRenderResourceInitialized()
{
    return !_uninitializedResourceSet.contains(this);
}

} // namespace ZE