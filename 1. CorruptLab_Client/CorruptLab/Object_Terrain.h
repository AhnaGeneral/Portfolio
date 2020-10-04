//-----------------------------------------------------------------------------
// File: Geometry.h
//-----------------------------------------------------------------------------
#pragma once

#include "Object.h"
#include "Mesh_Terrain.h"
#include "Object_Camera.h"
#include "Mesh.h"

// Terrain =========================================================================================================
class CHeightMapTerrain : public CGameObject
{
private:
	D3D12_GPU_DESCRIPTOR_HANDLE	    	m_d3dCbvGPUDescriptorHandle;
	CHeightMapImage                   * m_pHeightMapImage;

	XMFLOAT3					    	m_xmf3Scale;

	int							    	m_nWidth;
	int							    	m_nLength;

	CHeightMapGridMesh               ** m_ppMeshes;
	int							    	m_nMeshes;
	CTexture                          * m_pShadowMap;

	int									*m_ipcbHeightFactor;
	int									*m_iHeightFactor;
	ID3D12Resource						* m_pd3dHeightFactorBuffer;

public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* ShadowMap= NULL);
	virtual ~CHeightMapTerrain();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);

	void SetHeightFactor();
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }

	void SetMesh(int nIndex, CHeightMapGridMesh* pMesh);

	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);
};

