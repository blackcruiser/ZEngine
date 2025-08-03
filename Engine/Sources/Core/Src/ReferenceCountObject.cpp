#include "ReferenceCountObject.h"


namespace ZE {

/*
* ReferenceCountObject
*/
ReferenceCountObject::ReferenceCountObject() :
    _referenceCount(0)
{
}

ReferenceCountObject::~ReferenceCountObject()
{
}

void ReferenceCountObject::AddReference()
{
    _referenceCount++;
}

void ReferenceCountObject::RemoveReference()
{
    ZE_CHECK(_referenceCount);
    _referenceCount--;

    if (_referenceCount == 0)
    {
        Destroy();
    }
}

void ReferenceCountObject::Destroy()
{
    delete this;
}


/*
* ReferenceCountObjectPool
*/
ReferenceCountObjectPool::ReferenceCountObjectPool()
{
}

ReferenceCountObjectPool::~ReferenceCountObjectPool()
{
}

void ReferenceCountObjectPool::AddObject(ReferenceCountObject* object)
{
    _resourceList.emplace_back(object);
}

void ReferenceCountObjectPool::CleanupPool()
{
    for (auto iter = _resourceList.begin(); iter != _resourceList.end(); ++iter)
    {
        ReferenceCountObject* object = *iter;
        
        delete object;
    }

    _resourceList.clear();
}


/*
* ReferenceCountObjectPool
*/
PooledReferenceCountObject::PooledReferenceCountObject(ReferenceCountObjectPool* pool) :
    _pool(pool)
{
}

PooledReferenceCountObject::~PooledReferenceCountObject()
{
}

void PooledReferenceCountObject::Destroy()
{
    ZE_CHECK(_pool);

    _pool->AddObject(this);
}

}