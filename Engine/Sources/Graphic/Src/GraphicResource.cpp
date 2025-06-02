#pragma once

#include "GraphicResource.h"
#include "Core/ReferenceCountObject.h"


namespace ZE {

GraphicResource::GraphicResource(ReferenceCountObjectPool* deleter) :
    PooledReferenceCountObject(deleter)
{
}

GraphicResource::~GraphicResource()
{
}

}