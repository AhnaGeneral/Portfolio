#include "stdafx.h"
#include "Object_Terrain.h"
#include "Shader_Terrain.h"
#include "Material.h"
// Terrain ==================================================================================
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* ShadowMap)
{
	m_pd3dHeightFactorBuffer = NULL;
	m_ipcbHeightFactor = NULL;

	m_nWidth = nWidth; // 512
	m_nLength = nLength; // 512

	int cxQuadsPerBlock = nBlockWidth - 1; //16
	int czQuadsPerBlock = nBlockLength - 1; //16

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock; //512 / 16 = 32
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks; // 32*32 = 1024
	m_ppMeshes = new CHeightMapGridMesh * [m_nMeshes]; // 1024? 
	for (int i = 0; i < m_nMeshes; i++)
		m_ppMeshes[i] = NULL;

	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++) // z:0 ~ z:31
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++) //x:0 ~ x:31
		{
			xStart = x * (nBlockWidth - 1); // x * 8  :: 0 , 8 , 16, 24 .... 248 
			zStart = z * (nBlockLength - 1); // z * 8
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart,
				nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
			//std:: d
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CTexture* pTerrainTexture = new CTexture(12, RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/SplatAlpha 0.dds", 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/Terrain1_NM.dds", 1);

	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/Sand2.dds", 2);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/Sand_NM.dds", 3);

	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/DryStone_BC.dds", 4);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/DryStone_NM.dds", 5);

	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/Grass1_BC.dds", 6);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/Grass2_BC.dds", 7);

	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/Sand1.dds", 8);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/Grass1_NM.dds", 9);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/DryStone_HT.dds", 10);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/Sand_HT.dds", 11);


	m_pShadowMap = (CTexture*)ShadowMap;

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	CTerrainShader* pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTerrainShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 13);
	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, ROOT_PARAMETER_TERRAIN_TEX, true);
	pTerrainShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObjects, ncbElementBytes);
	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pShadowMap, ROOT_PARAMETER_SHADOWMAP, 0);


	SetShader(pTerrainShader);

	m_ppMaterials[0]->SetTexture(pTerrainTexture);

	SetCbvGPUDescriptorHandle(pTerrainShader->GetGPUCbvDescriptorStartHandle());
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage)
		delete m_pHeightMapImage;
	if (m_nMeshes > 0)
	{
		for (int i = 0; i < m_nMeshes; ++i)
			m_ppMeshes[i]->Release();
	}

	if (m_pd3dHeightFactorBuffer)
	{
		m_pd3dHeightFactorBuffer->Unmap(0, NULL);
		m_pd3dHeightFactorBuffer->Release();
	}

}

void CHeightMapTerrain::SetMesh(int nIndex, CHeightMapGridMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CHeightMapTerrain::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);

	int ncbElementBytes = ((sizeof(int) + 255) & ~255);
	m_pd3dHeightFactorBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dHeightFactorBuffer->Map(0, NULL, (void**)&m_ipcbHeightFactor);
	m_iHeightFactor = new int(2);
}

void CHeightMapTerrain::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pShadowMap->UpdateShaderVariables(pd3dCommandList);
}

void CHeightMapTerrain::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	CGameObject::UpdateShaderVariable(pd3dCommandList, pxmf4x4World);

	::memcpy(m_ipcbHeightFactor, m_iHeightFactor, sizeof(int));
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dHeightFactorBuffer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_RADIATIONLEVEL, d3dGpuVirtualAddress);
}

void CHeightMapTerrain::SetHeightFactor()
{
	*m_iHeightFactor += 1;
	if (*m_iHeightFactor > 2)
		*m_iHeightFactor = 0;
}



void CHeightMapTerrain::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	//OnPrepareRender();

	m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);

	if (nPipelineState == 0)
	{		
		m_ppMaterials[0]->UpdateShaderVariable(pd3dCommandList);
		UpdateShaderVariables(pd3dCommandList);

	}

	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	XMFLOAT3 CameraPos = pCamera->GetPosition();
	bool bRender = true;
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i])
			{
				if (pCamera->m_boundingFrustum.Intersects(m_ppMeshes[i]->m_boundingbox))
				{
					m_ppMeshes[i]->Render(pd3dCommandList, i);
				}
			}
		}
	}
}

void CHeightMapTerrain::ReleaseUploadBuffers()
{
	if (m_nMeshes)
		for (int i = 0; i < m_nMeshes; ++i)
			m_ppMeshes[i]->ReleaseUploadBuffers();

	m_ppMaterials[0]->ReleaseUploadBuffers();

}
