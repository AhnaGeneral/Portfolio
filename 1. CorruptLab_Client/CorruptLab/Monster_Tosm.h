#pragma once
#include "Monster.h"
class CTosm :
    public CMonster
{
private:
    float m_fIdleTime = 2; private:
        CGameObject* m_pNeck = NULL;
public:
    CTosm();
    ~CTosm() {}

    virtual void Initialize(XMFLOAT3 FiledPos, int iAtt);

    virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL, int iNum = 0);

    virtual void GoodUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL, void* pContext = NULL);
    virtual void BadUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL, void* pContext = NULL);

};

