#pragma once
#include "Monster.h"

class CYangmal :
	public CMonster
{
public:
	CYangmal() {}
	~CYangmal() {}

	virtual void BadUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL, void* pContext = NULL);
	virtual void GoodUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL, void* pContext = NULL);
	
	void Rush(XMFLOAT3& pos, float fTimeElapsed, float Speed, CHeightMapTerrain* pTerrain);

	virtual void OnInitialize();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL, int iNum = 0);
private:
	CGameObject* m_pNeck = NULL;
};
