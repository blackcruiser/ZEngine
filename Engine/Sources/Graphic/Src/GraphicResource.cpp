#pragma once

#include "GraphicResource.h"


namespace ZE {

std::vector<GraphicResource*> GraphicResource::pendingDeleteResources;

GraphicResource::GraphicResource() :
    _frameNumber(0)
{
}

GraphicResource::~GraphicResource()
{
}

void GraphicResource::MarkUsed(int32_t frameNumber)
{
    _frameNumber = frameNumber;
}

int32_t GraphicResource::GetUsedFrameNumber()
{
    return _frameNumber;
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