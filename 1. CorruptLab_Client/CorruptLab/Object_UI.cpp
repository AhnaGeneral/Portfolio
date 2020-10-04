#include "stdafx.h"
#include "Shader.h"
#include "Shader_Minimap.h"
#include "shader_ObjHP.h"
#include "Mgr_Item.h"


CMRTUI::CMRTUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nobjectID = 0;
}

CMRTUI::~CMRTUI()
{
}

void CMRTUI::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

	UINT getobjectID = m_nobjectID;
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 4, &getobjectID, 16);

}

void CMRTUI::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	OnPrepareRender();
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, 0);
}

void CMRTUI::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	m_pd3dcbGameObjects = NULL;

	//if (m_pcbMappedGameObjects) delete m_pcbMappedGameObjects;
}

//void CMRTUI::ReleaseUploadBuffers()
//{
//}

void CMRTUI::SetObjectID(UINT objectID)
{
	m_nobjectID = objectID;
}

void CMRTUI::Set2DPosition(float x, float y)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;

	UpdateTransform(NULL);
}

// [ CUI_MiniMap ] =======================================================================================================
CUI_MiniMap::CUI_MiniMap()
{
}

CUI_MiniMap::CUI_MiniMap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

void CUI_MiniMap::InterLinkShaderTexture(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* Texture)
{
	CMinimapShader* pShader = new CMinimapShader();
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 3, 3);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);

	CTexture* pMinimapTex = new CTexture(3, RESOURCE_TEXTURE2D, 0);
	pMinimapTex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UserInterface/MiniMap/Map.dds", 0);
	pMinimapTex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UserInterface/MiniMap/Map_Fog1.dds", 1);
	pMinimapTex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UserInterface/MiniMap/Map_Fog2.dds", 2);
	
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pMinimapTex, ROOT_PARAMETER_ITEM_TEX, true); 
	SetShader(pShader);
	m_ppMaterials[0]->SetTexture(pMinimapTex);
}

void CUI_MiniMap::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(XMFLOAT3) + 255) & ~255); //256의 배수  
	m_pd3dcbPlayerPosition = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbPlayerPosition->Map(0, NULL, (void**)&m_pcbPlayerPosition);
}

void CUI_MiniMap::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera);
	m_ppMaterials[0]->UpdateShaderVariable(pd3dCommandList);

	OnPrepareRender();
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, 0);
}

void CUI_MiniMap::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

	memcpy(m_pcbPlayerPosition, m_PlayerPosition, sizeof(XMFLOAT3));
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPlayerPosition->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_PLAYER_POS, d3dGpuVirtualAddress);

}

void CUI_MiniMap::ReleaseShaderVariables()
{
	CMRTUI::ReleaseShaderVariables(); 

	if (m_pd3dcbPlayerPosition)
	{
		m_pd3dcbPlayerPosition->Unmap(0, NULL);
		m_pd3dcbPlayerPosition->Release();
	}
	m_pd3dcbPlayerPosition = NULL;

}

//void CUI_MiniMap::ReleaseUploadBuffers()
//{
//}


// [ CUI_HP ] ==========================================================================================================
CUI_Root::CUI_Root()
{
}

CUI_Root::CUI_Root(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

void CUI_Root::InterLinkShaderTexture(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pShader, void* pTexture)
{
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateMaterial();

	if (pTexture)
	{
		m_ppMaterials[0]->SetTexture((CTexture*)pTexture);
	}
}


void CUI_Root::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(float) + 255) & ~255); //256의 배수  
	m_pd3dcbAlpha= ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbAlpha->Map(0, NULL, (void**)&m_pcbAlpha);
}

void CUI_Root::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariable(pd3dCommandList); 

	OnPrepareRender();
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, 0);
}

void CUI_Root::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

	//std::cout << *m_Alpha << std::endl; 
	memcpy(m_pcbAlpha, m_Alpha, sizeof(float));
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbAlpha->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_PLAYER_HP, d3dGpuVirtualAddress);

}

void CUI_Root::ReleaseShaderVariables()
{
	CMRTUI::ReleaseShaderVariables();
}

// [ CUI_Inventory ] ======================================================================================================
CUI_ITem::CUI_ITem()
{

}

CUI_ITem::CUI_ITem(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

void CUI_ITem::InterLinkShaderTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* Texture)
{
	//SetShader((CShader*)Texture);
	//m_ppMaterials[0]->SetTexture((CTexture*)Texture);
}

void CUI_ITem::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);

	UINT ncbElementBytes = ((sizeof(CB_ITEM) + 255) & ~255); //256의 배수  
	m_pd3dcbItemReact = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbItemReact->Map(0, NULL, (void**)&m_pcbItemReact);

}

void CUI_ITem::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	//m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera);
	//m_ppMaterials[0]->UpdateShaderVariable(pd3dCommandList);

	OnPrepareRender();
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, 0);
}

void CUI_ITem::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	m_ItemReact.m_fTIme += 0.01f;
	if (m_ItemReact.m_fTIme > 1.0f)
	{
		m_ItemReact.m_fTIme = 0.0f;
	}

	//안개 컨트롤 ===========================================================
	if (m_ItemReact.m_f4iTemCount.w == 1.0f && m_ItemReact.m_fTIme > 0.96)
	{
		m_ItemReact.m_Control = PROGRESS_FILED1;
	}
	if (m_ItemReact.m_f4iTemCount.w == 2.0f && m_ItemReact.m_fTIme > 0.96)
	{
		m_ItemReact.m_Control = PROGRESS_FILED2;
	}
	//=====================================================================
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

	UINT getobjectID = m_nobjectID;
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 4, &getobjectID, 16);
	//cout << m_ItemReact.m_f4iTemCount.w << endl;
	memcpy(m_pcbItemReact, &m_ItemReact, sizeof(CB_ITEM));
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbItemReact->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_ITEM_REACT, d3dGpuVirtualAddress);

}

void CUI_ITem::ReleaseShaderVariables()
{
	CMRTUI::ReleaseShaderVariables();

	if (m_pd3dcbItemReact)
	{
		m_pd3dcbItemReact->Unmap(0, NULL);
		m_pd3dcbItemReact->Release();
	}
	m_pd3dcbItemReact = NULL;

}

void CUI_ITem::SetItemCount(XMFLOAT4 itemCount)
{
	m_ItemReact.m_f4iTemCount.x = itemCount.x;
	m_ItemReact.m_f4iTemCount.y = itemCount.y;
	m_ItemReact.m_f4iTemCount.z = itemCount.z;
	m_ItemReact.m_f4iTemCount.w = itemCount.w;
}

CUI_HP::CUI_HP()
{
}

CUI_HP::CUI_HP(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

void CUI_HP::InterLinkShaderTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature, void* Texture)
{

	CShader_ObjHP* pShader = new CShader_ObjHP();
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 2, 1);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);

	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, (CTexture*)Texture, ROOT_PARAMETER_HP_TEX, true);
	SetShader(pShader);
	m_ppMaterials[0]->SetTexture((CTexture*)Texture);
}

void CUI_HP::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);

	UINT ncbElementBytes = ((sizeof(float) + 255) & ~255); //256의 배수  
	m_pd3dcbPlayerHP = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbPlayerHP->Map(0, NULL, (void**)&m_pcbPlayerHP);

}

void CUI_HP::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera);
	m_ppMaterials[0]->UpdateShaderVariable(pd3dCommandList);

	OnPrepareRender();
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, 0);
}

void CUI_HP::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);
	//std::cout << *m_PlayerHP << std::endl;

	memcpy(m_pcbPlayerHP, m_PlayerHP, sizeof(float));
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPlayerHP->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_PLAYER_HP, d3dGpuVirtualAddress);
}

void CUI_HP::ReleaseShaderVariables()
{
	CMRTUI::ReleaseShaderVariables();

	if (m_pd3dcbPlayerHP)
	{
		m_pd3dcbPlayerHP->Unmap(0, NULL);
		m_pd3dcbPlayerHP->Release();
	}
	m_pd3dcbPlayerHP = NULL;

}

// [ 방사능 수치 카운터 ] ========================================================================================

CUI_RaditaionLevel::CUI_RaditaionLevel()
{
}

CUI_RaditaionLevel::CUI_RaditaionLevel(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

void CUI_RaditaionLevel::InterLinkShaderTexture(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pShader, void* pTexture)
{

	//m_pMeshTen = new CTriangleRect(pd3dDevice, pd3dCommandList, 40, 50, 0.0f, 1.0f);
	//m_pMeshOne = new CTriangleRect(pd3dDevice, pd3dCommandList, 40, 50, 0.0f, 1.0f);

	
	CreateMaterial();

	if (pTexture)
	{
		m_ppMaterials[0]->SetTexture((CTexture*)pTexture);
	}
}


void CUI_RaditaionLevel::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pcbRadiationNum = new float(0);
	UINT ncbElementBytes = ((sizeof(float) + 255) & ~255); //256의 배수  
	m_pd3dcbRadiationLevel = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbRadiationLevel->Map(0, NULL, (void**)&m_pcbRadiationNum);
}

void CUI_RaditaionLevel::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariable(pd3dCommandList);

	OnPrepareRender();
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_pMesh) m_pMesh->Render(pd3dCommandList, 0);
}

void CUI_RaditaionLevel::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

	memcpy(m_pcbRadiationNum, &m_RadiationNumber, sizeof(float));
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbRadiationLevel->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_PLAYER_HP, d3dGpuVirtualAddress);

}

void CUI_RaditaionLevel::ReleaseShaderVariables()
{
	CMRTUI::ReleaseShaderVariables();
}

void CUI_RaditaionLevel::SetRadiationNumber(float num)
{
	m_RadiationNumber = num;
}

CUI_MonsterHP::CUI_MonsterHP()
{
	m_pd3dcbMonsterHP = NULL;
	m_MonsterHP = NULL;
	m_pcbMonsterHP = NULL;
}

CUI_MonsterHP::CUI_MonsterHP(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	InstanceInfo.m_xmf3Position = XMFLOAT3(0, 0, 0);
	InstanceInfo.m_xmf2Size = XMFLOAT2(0, 0);
}

void CUI_MonsterHP::SetInstanceInfo( XMFLOAT2 Scale, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	InstanceInfo.m_xmf3Position = XMFLOAT3(0, 20.0f  * Scale.x, 0);
	InstanceInfo.m_xmf2Size = Scale;

	m_pd3dPositionBuffer =
		::CreateBufferResource(pd3dDevice, pd3dCommandList, &InstanceInfo, sizeof(GS_BILLBOARD_INSTANCE),
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(GS_BILLBOARD_INSTANCE);
	m_d3dPositionBufferView.SizeInBytes = sizeof(GS_BILLBOARD_INSTANCE);
}

void CUI_MonsterHP::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);

	UINT ncbElementBytes = ((sizeof(int) + 255) & ~255); //256의 배수  
	m_pd3dcbMonsterHP = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbMonsterHP->Map(0, NULL, (void**)&m_MonsterHP);
}

void CUI_MonsterHP::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	m_ppMaterials[0]->m_pShader->Render(pd3dCommandList,pCamera);
	m_ppMaterials[0]->UpdateShaderVariable(pd3dCommandList);
	UpdateShaderVariable(pd3dCommandList,&m_xmf4x4World);
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[1] = { m_d3dPositionBufferView };
	pd3dCommandList->IASetVertexBuffers(0, _countof(pVertexBufferViews), pVertexBufferViews);
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	pd3dCommandList->DrawInstanced(1, 1, 0, 0);
}

void CUI_MonsterHP::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);
	//std::cout << *m_PlayerHP << std::endl;

	memcpy(m_MonsterHP, m_pcbMonsterHP, sizeof(int));
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbMonsterHP->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_RADIATIONLEVEL, d3dGpuVirtualAddress);
}
