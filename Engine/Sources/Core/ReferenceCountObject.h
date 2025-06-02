#pragma once

#include "CoreDefines.h"
#include "Debug/AssertionMacros.h"

#include <cstdint>
#include <list>


namespace ZE {

template<typename T>
class TReferenceCountPtr
{
public:
    TReferenceCountPtr(T* ptr) :
        _ptr(ptr)
    {
        if (ptr)
        {
            ptr->AddReference();
        }
    }

    TReferenceCountPtr(const TReferenceCountPtr& other) :
        _ptr(other._ptr)
    {
        if (_ptr)
        {
            _ptr->AddReference();
        }
    }

    ~TReferenceCountPtr()
    {
        if (_ptr)
        {
            ZE_CHECK(_ptr->GetReferenceCount());
            _ptr->RemoveReference();
        }
    }

private:
    T* _ptr; 
};

class ReferenceCountObject
{
public:
    ReferenceCountObject();
    virtual ~ReferenceCountObject();

    void AddReference();
    void RemoveReference();

    uint32_t GetReferenceCount();

    virtual void Destroy();

private:
    uint32_t _referenceCount;
};

class ReferenceCountObjectPool
{
public:
    ReferenceCountObjectPool();
    ~ReferenceCountObjectPool();

    void AddObject(ReferenceCountObject* object);

    void DelayDestroy();

private:
    std::list<ReferenceCountObject*> _resourceList;
};

class PooledReferenceCountObject : public ReferenceCountObject
{
public:
    PooledReferenceCountObject(ReferenceCountObjectPool* pool);
    virtual ~PooledReferenceCountObject();

    virtual void Destroy() override;

private:
    ReferenceCountObjectPool* _pool;
};

} // namespace ZE
