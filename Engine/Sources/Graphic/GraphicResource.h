#pragma once

#include "CoreDefines.h"
#include "Core/ReferenceCountObject.h"


namespace ZE {

class GraphicResource : public PooledReferenceCountObject
{
public:
    GraphicResource(ReferenceCountObjectPool* deleter);
    virtual ~GraphicResource();
};

class GraphicResourceDeleter : public ReferenceCountObjectPool
{
};

}