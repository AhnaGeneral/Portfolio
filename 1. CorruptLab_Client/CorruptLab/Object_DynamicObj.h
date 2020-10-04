#pragma once
#include "Object_StaticObj.h"
class CDynamicObject : public CStaticObject
{
public:
	virtual void OnInitialize();
	virtual void Update(float fElapsedTime) {}
};

