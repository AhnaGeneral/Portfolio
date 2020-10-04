#include "Mgr_Item.h"
#include "Object_RotatingItem.h"

CItemMgr* CItemMgr::m_pInstance = NULL;

void CItemMgr::Initialize(ID3D12Device* pd3dDevice, 
	    ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pRotatingItem = new CRotatingItem(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pRotatingItem->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pPasswordInDesk = new CPasswordInDesk(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPasswordInDesk->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//m_cb_item = new CB_ITEM;
}

void CItemMgr::BillboardUIRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	m_pRotatingItem->Render(pd3dCommandList, pCamera);
	m_pPasswordInDesk->Render(pd3dCommandList, pCamera);
}

void CItemMgr::Update(float fElapsedTime)
{
	m_pRotatingItem->Animate(fElapsedTime, NULL);
	m_pPasswordInDesk->Animate(fElapsedTime, NULL);
}

void CItemMgr::GetPassword(XMFLOAT3 Pos)
{
	Pos.y += 3.f;
	m_pPasswordInDesk->SetPosition(Pos);
	m_pPasswordInDesk->m_bAnimate = true;
}

void CItemMgr::GetItem(int iType, XMFLOAT3 Pos)
{
	Pos.y += 2.f;
	m_pRotatingItem->m_nItemType = iType;
	m_pRotatingItem->SetPosition(Pos);
	m_pRotatingItem->m_bAnimate = true;
}

void CItemMgr::GiveItemToPlayer(int iType)
{
	if (iType == ITEM_TYPE_HANDLIGHT)
		m_fItemNums.x += 1.0f;

	if (iType == ITEM_TYPE_HPKIT)
		m_fItemNums.y += 1.0f;

	if (iType == ITEM_TYPE_PILL)
		m_fItemNums.z += 1.0f;

	if (iType == ITEM_TYPE_MAPSEGMENT)
		m_fItemNums.w += 1.0f;

}

void CItemMgr::UseItemToPlayer(int iType)
{
	if (iType == ITEM_TYPE_HANDLIGHT)
		if (m_fItemNums.x > 0.0f)
			m_fItemNums.x -= 1.0f;

	if (iType == ITEM_TYPE_HPKIT)
		if (m_fItemNums.y> 0.0f)
			m_fItemNums.y -= 1.0f;

	if (iType == ITEM_TYPE_PILL)
		if (m_fItemNums.z > 0.0f)
			m_fItemNums.z -= 1.0f;

	if (iType == ITEM_TYPE_MAPSEGMENT)
		if (m_fItemNums.w > 0.0f)
			m_fItemNums.w -= 1.0f;

}

void CItemMgr::Destroy()
{
	if (m_pRotatingItem)
	{
		m_pRotatingItem->ReleaseShaderVariables();
		m_pRotatingItem->ReleaseUploadBuffers();
		m_pRotatingItem->Release();
	}
	if (m_pPasswordInDesk)
	{
		m_pPasswordInDesk->ReleaseShaderVariables();
		m_pPasswordInDesk->ReleaseUploadBuffers();
		m_pPasswordInDesk->Release();
	}

	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}

}
