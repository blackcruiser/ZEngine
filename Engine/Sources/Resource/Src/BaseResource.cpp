#include "BaseResource.h"

namespace ZE {

BaseResource::BaseResource() : _isLoaded(false)
{
}

BaseResource::~BaseResource()
{
    if (IsLoaded())
        Unload();
}

bool BaseResource::IsLoaded()
{
    return _isLoaded;
}

} // namespace ZE