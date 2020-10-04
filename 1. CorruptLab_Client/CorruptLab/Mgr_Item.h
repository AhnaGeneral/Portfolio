#pragma once
#include "stdafx.h"
using namespace std;

class CPlayer;
class CGameScene;
class CCamera;
class CRotatingItem;

class CItemMgr
{
private:
	static CItemMgr    * m_pInstance;
	CRotatingItem      * m_pRotatingItem = NULL;
	CRotatingItem		* m_pPasswordInDesk = NULL;

	XMFLOAT4 m_fItemNums = { 0.0f, 0.0f, 0.0f, 0.0f };
	int	m_iItemNum[4] = {0,0,0,0};
	int m_react = 0;

public:
	void Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignatureCPlayer);
	void BillboardUIRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void Update(float fElapsedTime);
	
	
	void GetPassword(XMFLOAT3 Pos);
	void GetItem(int iType, XMFLOAT3 Pos);
	void GiveItemToPlayer(int iType);
	void UseItemToPlayer(int iType); 
	void SetReactItem(int iType) { m_react = iType; }
	int GetReactIten() { return m_react; }
	XMFLOAT4 GetItemNums(){ return m_fItemNums; }
	
	static CItemMgr* GetInstance(void)
	{
		if (m_pInstance == NULL)
			m_pInstance = new CItemMgr();
		return m_pInstance;
	}
	void Destroy();
	
};

