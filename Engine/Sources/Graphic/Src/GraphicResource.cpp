#pragma once

#include "GraphicResource.h"


namespace ZE {

std::vector<GraphicResource*> GraphicResource::pendingDeleteResources;

GraphicResource::GraphicResource() :
    _executeCounter(0)
{
}

GraphicResource::~GraphicResource()
{
}

void GraphicResource::MarkUsed(int32_t executeCounter)
{
    _executeCounter = executeCounter;
}

int32_t GraphicResource::GetUsedExecuteCounter()
{
    return _executeCounter;
}

std::vector<GraphicResource*>& GraphicResource::GetPendingDeleteResources()
{
    return pendingDeleteResources;
}

void GraphicResourceDeleter::operator()(GraphicResource* resource)
 {
     DelayDelete(resource);
 }

 void GraphicResourceDeleter::DelayDelete(GraphicResource* resource)
 {
    GraphicResource::GetPendingDeleteResources().push_back(resource);
 }

}