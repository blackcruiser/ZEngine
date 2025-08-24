#pragma once

#include "CoreDefines.h"

#include <list>


namespace ZE {

template<typename T>
class ObjectPool
{
public:
    ObjectPool() = default;
    ObjectPool(ObjectPool&) = delete;
    ObjectPool(const ObjectPool&) = delete;

    void AddResource(T* resource)
    {
        _resourceList.push_back(resource);
    }

    void Cleanup()
    {
        for (T* resource : _resourceList)
            delete resource;

        _resourceList.clear();
    }

private:
    std::list<T*> _resourceList;
};

}