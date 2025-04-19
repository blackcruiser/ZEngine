#pragma once
#include "CoreDefines.h"
#include "CoreTypes.h"

#include <unordered_set>


namespace ZE {

class RenderGraph;

class RenderResource
{
public:
    static std::unordered_set<RenderResource*>& GetAll();
    static std::unordered_set<RenderResource*>& GetUninitialized();

    static void InitializeRenderResources(TPtr<RenderGraph> renderGraph);
    static void CleanupRenderResources(TPtr<RenderGraph> renderGraph);

private:
    static std::unordered_set<RenderResource*> _resourceSet;
    static std::unordered_set<RenderResource*> _uninitializedResourceSet;

public:
    RenderResource();
    ~RenderResource();

    virtual void InitRenderResource(TPtr<RenderGraph> renderGraph);
    virtual void CleanupRenderResource(TPtr<RenderGraph> renderGraph);

    bool IsRenderResourceInitialized();
};

}