#pragma once
#include "Monster.h"
class CBoss :
    public CMonster
{
public:
    virtual void Update(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL, void* pContext = NULL);

};

