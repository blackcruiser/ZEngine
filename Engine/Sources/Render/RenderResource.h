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

    static void InitializeRenderResources(RenderGraph* renderGraph);
    static void CleanupGraphics();

private:
    static std::unordered_set<RenderResource*> _resourceSet;
    static std::unordered_set<RenderResource*> _uninitializedResourceSet;

public:
    RenderResource();
    ~RenderResource();

    virtual void Init();
    virtual void Cleanup();

    virtual void InitGraphic(RenderGraph* renderGraph);
    virtual void CleanupGraphic();

    bool IsRenderResourceInitialized();
};

void InitRenderResourceGameThread(RenderResource* resource);

void CleanupRenderResourceGameThread(RenderResource* resouce);

}