#include "Object_Floor.h"
#include "Shader_Floor.h"

CFloor::CFloor(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CTriangleRect* pNoise = new CTriangleRect(pd3dDevice, pd3dCommandList, 300.0f, 300.0f, 0.0f, 1.0f);
	SetMesh(pNoise);

	CShader* pShader = new CFloorShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 2);
	SetShader(pShader,2);

	CTexture* pTileTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTileTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/Tiles_BC.dds", 0);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTileTexture, ROOT_PARAMETER_ALBEDO_TEX, true);

	m_ppMaterials[0]->SetTexture(pTileTexture,0);

	pTileTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTileTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Terrain/Tiles_NM.dds", 0);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTileTexture, ROOT_PARAMETER_NORMAL_TEX, true);

	m_ppMaterials[0]->SetTexture(pTileTexture, 1);
	Rotate(90, 0, 0);
}

CFloor::~CFloor()
{
}

void CFloor::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
	if (m_ppMaterials[0]->m_pShader)
		m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);

	if (nPipelineState == 0)
		m_ppMaterials[0]->UpdateShaderVariable(pd3dCommandList);

	m_pMesh->Render(pd3dCommandList,0);
}
