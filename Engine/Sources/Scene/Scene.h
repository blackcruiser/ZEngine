#pragma once

#include "CoreDefines.h"

class TESceneObject;

class TEScene
{
public:
    static TEPtr<TEScene> CreateSampleScene();

public:
    TEScene();

    void AddObject(TEPtr<TESceneObject> object);
    const TEPtrArr<TESceneObject> &GetObjects();

private:
    TEPtrArr<TESceneObject> _objects;
};