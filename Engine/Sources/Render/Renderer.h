#pragma once

#include "CoreDefines.h"


class TEScene;


class TERendererInterface
{
public:
	virtual void RenderFrame(TEPtr<TEScene> scene) = 0;

};