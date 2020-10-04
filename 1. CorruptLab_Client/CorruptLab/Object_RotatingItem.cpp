#include "Object_RotatingItem.h"
#include "Shader_BillboardUI.h"
#include "Mgr_Item.h"

CRotatingItem::CRotatingItem(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	for (int i = 0; i < 4; i++)
		m_pItemTextures[i] = new CTexture(1, RESOURCE_TEXTURE2D, 0);

	m_pItemTextures[ITEM_TYPE_HANDLIGHT]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UserInterface/Inventory/HandLight.dds", 0);
	m_pItemTextures[ITEM_TYPE_HPKIT]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UserInterface/Inventory/HP_Kit.dds", 0);
	m_pItemTextures[ITEM_TYPE_PILL]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UserInterface/Inventory/Pill.dds", 0);
	m_pItemTextures[ITEM_TYPE_MAPSEGMENT]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UserInterface/Inventory/MapSegment.dds", 0);

	CTriangleRect* pMesh = new CTriangleRect(pd3dDevice, pd3dCommandList, 3, 3, 0, 1.f);
	SetMesh(pMesh);

	CBillboardUIShader* pShader = new CBillboardUIShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 4);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pItemTextures[ITEM_TYPE_HANDLIGHT], ROOT_PARAMETER_ALBEDO_TEX,0);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pItemTextures[ITEM_TYPE_HPKIT], ROOT_PARAMETER_ALBEDO_TEX, 0);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pItemTextures[ITEM_TYPE_PILL], ROOT_PARAMETER_ALBEDO_TEX, 0);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pItemTextures[ITEM_TYPE_MAPSEGMENT], ROOT_PARAMETER_ALBEDO_TEX, 0);

	SetShader(pShader);
}

void CRotatingItem::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	if (!m_bAnimate) return;

	m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera);
	
	m_pItemTextures[m_nItemType]->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, 0);
}

CRotatingItem::~CRotatingItem()
{
	if (m_pItemTextures)
	{
		for (int i = 0; i < m_nItemType; ++i)
		{
			m_pItemTextures[i]->ReleaseShaderVariables();
			m_pItemTextures[i]->ReleaseUploadBuffers();
			m_pItemTextures[i]->Release();
		}
	}
}

void CRotatingItem::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (!m_bAnimate) return;
	
	m_fElapsedTIme += fTimeElapsed;
	Rotate(0, fTimeElapsed * 200, 0);
	MoveUp(fTimeElapsed * 6);

	if (m_fElapsedTIme > 1.5f)
	{
		m_fElapsedTIme = 0.0f;
		m_bAnimate = false;
		CItemMgr::GetInstance()->GiveItemToPlayer(m_nItemType);
	}
	UpdateTransform(NULL);
}

CPasswordInDesk::CPasswordInDesk(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{

	m_pItemTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0);

	m_pItemTextures[0]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UserInterface/Inventory/CodingPassword.dds", 0);

	CTriangleRect* pMesh = new CTriangleRect(pd3dDevice, pd3dCommandList, 3, 3, 0, 1.f);
	SetMesh(pMesh);

	CBillboardUIShader* pShader = new CBillboardUIShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 1);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pItemTextures[0], ROOT_PARAMETER_ALBEDO_TEX, 0);

	SetShader(pShader);
}

void CPasswordInDesk::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (!m_bAnimate) return;

	m_fElapsedTIme += fTimeElapsed;
	Rotate(0, fTimeElapsed * 150, 0);
	MoveUp(fTimeElapsed * 4.5f);

	if (m_fElapsedTIme > 1.5f)
	{
		m_fElapsedTIme = 0.0f;
		m_bAnimate = false;
	}
	UpdateTransform(NULL);
}
