#pragma once
#include "Mesh.h"

class CGameObject;

#define SKINNED_ANIMATION_BONES		128
// CSkinnedMesh-----------------------------------------------------------------------------------
class CSkinnedMesh : public CStandardMesh
{
public:
	CSkinnedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CSkinnedMesh();

protected:
	char							m_pstrSkinnedMeshName[64] = { 0 };

	int								m_nBonesPerVertex;

	XMUINT4                       * m_pxmu4BoneIndices; 
	ID3D12Resource                * m_pd3dBoneIndexBuffer;
	ID3D12Resource                * m_pd3dBoneIndexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneIndexBufferView;

	XMFLOAT4                      * m_pxmf4BoneWeights;
	ID3D12Resource                * m_pd3dBoneWeightBuffer;
	ID3D12Resource                * m_pd3dBoneWeightUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneWeightBufferView;

public:
	int								 m_nSkinningBones;

	char                           (*m_ppstrSkinningBoneNames)[64]; 
	XMFLOAT4X4                     * m_pxmf4x4BindPoseBoneOffsets;
								 
	CGameObject                   ** m_ppSkinningBoneFrameCaches;//
								 
	ID3D12Resource                 * m_pd3dcbBindPoseBoneOffsets = NULL;
	XMFLOAT4X4                     * m_pcbxmf4x4MappedBindPoseBoneOffsets = NULL; 
	
	
								 
	ID3D12Resource                 * m_pd3dcbBoneTransforms;
	XMFLOAT4X4                     * m_pcbxmf4x4BoneTransforms;

public:

	void PrepareSkinning(CGameObject* pModelRootObject);
	void LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);
};

