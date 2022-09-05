#pragma once

namespace TE {

class BaseResource
{
public:
    BaseResource();
    ~BaseResource();

    virtual bool IsLoaded();
    virtual void Load(){};
    virtual void Unload(){};

protected:
    bool _isLoaded;
};

} // namespace TE