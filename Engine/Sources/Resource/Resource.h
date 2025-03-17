#pragma once

namespace TE {

class Resource
{
public:
    virtual void Load();
    virtual void Unload();

private:
    bool _isLoaded;
};

}