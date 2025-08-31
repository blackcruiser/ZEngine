#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vector>


namespace ZE {

class GraphicResource
{
public:
    GraphicResource();
    virtual ~GraphicResource();

    void MarkUsed(int32_t executeCounter);
    int32_t GetUsedExecuteCounter();

private:
    int32_t _executeCounter;

public:
    static std::vector<GraphicResource*>& GetPendingDeleteResources();

protected:
    static std::vector<GraphicResource*> pendingDeleteResources;
};

class GraphicResourceDeleter
{
public:
    static void DelayDelete(GraphicResource* resource);

    void operator()(GraphicResource* resource);
};

template <typename T, class... ArgTypes>
requires std::is_base_of_v<GraphicResource, T>
TPtr<T> NewGraphicResource(ArgTypes&&... Args)
{
    return TPtr<T>(new T(std::forward<ArgTypes>(Args)...), GraphicResourceDeleter());
}

template <typename T, class... ArgTypes>
requires std::is_base_of_v<GraphicResource, T>
T* NewTempGraphicResource(ArgTypes&&... Args)
{
    T* ptr = new T(std::forward<ArgTypes>(Args)...);
    GraphicResourceDeleter::DelayDelete(ptr);
    return ptr;
}

}