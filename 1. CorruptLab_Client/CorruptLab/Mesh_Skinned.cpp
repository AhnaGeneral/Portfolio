#include "stdafx.h"
#include "Mesh_Skinned.h"
#include "Object.h"

// CSkinnedMesh-----------------------------------------------------------------------------------
CSkinnedMesh::CSkinnedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) 
	: CStandardMesh(pd3dDevice, pd3dCommandList)
{
	m_nBonesPerVertex = 4;

	m_pxmu4BoneIndices = NULL;
	m_pd3dBoneIndexBuffer = NULL;
	m_pd3dBoneIndexUploadBuffer = NULL;
	m_d3dBoneIndexBufferView;

	m_pxmf4BoneWeights = NULL;
	m_pd3dBoneWeightBuffer = NULL;
	m_pd3dBoneWeightUploadBuffer = NULL;

	m_nSkinningBones = 0;
	m_pxmf4x4BindPoseBoneOffsets = NULL;

	m_ppSkinningBoneFrameCaches = NULL;

	/*m_pd3dcbBoneOffsets = NULL;
	m_pcbxmf4x4BoneOffsets = NULL;*/

	m_pd3dcbBoneTransforms = NULL;
	m_pcbxmf4x4BoneTransforms = NULL;
}

CSkinnedMesh::~CSkinnedMesh()
{
	if (m_pd3dBoneIndexBuffer) m_pd3dBoneIndexBuffer->Release();
	if (m_pxmu4BoneIndices) delete[] m_pxmu4BoneIndices;

	if (m_pd3dBoneWeightBuffer) m_pd3dBoneWeightBuffer->Release();
	if (m_pxmf4BoneWeights) delete[] m_pxmf4BoneWeights;

	if (m_ppSkinningBoneFrameCaches) delete[] m_ppSkinningBoneFrameCaches;
	if (m_ppstrSkinningBoneNames) delete[] m_ppstrSkinningBoneNames;

	if (m_pxmf4x4BindPoseBoneOffsets) delete[] m_pxmf4x4BindPoseBoneOffsets;

	if (m_pd3dcbBindPoseBoneOffsets) m_pd3dcbBindPoseBoneOffsets = NULL;
	if (m_pcbxmf4x4BoneTransforms) m_pcbxmf4x4BoneTransforms = NULL;

	ReleaseShaderVariables();
}

void CSkinnedMesh::LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	BYTE nStrLength = 0;

	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(m_pstrSkinnedMeshName, sizeof(char), nStrLength, pInFile);
	m_pstrSkinnedMeshName[nStrLength] = '\0';

	for (;;)
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<BonesPerVertex>:"))
		{
			nReads = (UINT)::fread(&m_nBonesPerVertex, sizeof(int), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<BoneNames>:"))
		{
			nReads = (UINT)::fread(&m_nSkinningBones, sizeof(int), 1, pInFile);
			if (m_nSkinningBones > 0)
			{
				m_ppstrSkinningBoneNames = new char[m_nSkinningBones][64];
				m_ppSkinningBoneFrameCaches = new CGameObject * [m_nSkinningBones];
				for (int i = 0; i < m_nSkinningBones; i++)
				{
					nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
					nReads = (UINT)::fread(m_ppstrSkinningBoneNames[i], sizeof(char), nStrLength, pInFile);
					m_ppstrSkinningBoneNames[i][nStrLength] = '\0';

					m_ppSkinningBoneFrameCaches[i] = NULL;
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneOffsets>:"))
		{
			m_nSkinningBones = ::ReadIntegerFromFile(pInFile);
			if (m_nSkinningBones > 0)
			{
				m_pxmf4x4BindPoseBoneOffsets = new XMFLOAT4X4[m_nSkinningBones];
				nReads = (UINT)::fread(m_pxmf4x4BindPoseBoneOffsets, sizeof(XMFLOAT4X4), m_nSkinningBones, pInFile);

				UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
				m_pd3dcbBindPoseBoneOffsets = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
				m_pd3dcbBindPoseBoneOffsets->Map(0, NULL, (void**)&m_pcbxmf4x4MappedBindPoseBoneOffsets);

				for (int i = 0; i < m_nSkinningBones; i++)
				{
					XMStoreFloat4x4(&m_pcbxmf4x4MappedBindPoseBoneOffsets[i], XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4BindPoseBoneOffsets[i])));
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneWeights>:"))
		{
			m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

			nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
			if (m_nVertices > 0)
			{
				m_pxmu4BoneIndices = new XMUINT4[m_nVertices];
				m_pxmf4BoneWeights = new XMFLOAT4[m_nVertices];

				nReads = (UINT)::fread(m_pxmu4BoneIndices, sizeof(XMUINT4), m_nVertices, pInFile);
				m_pd3dBoneIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmu4BoneIndices,
					sizeof(XMUINT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneIndexUploadBuffer);

				m_d3dBoneIndexBufferView.BufferLocation = m_pd3dBoneIndexBuffer->GetGPUVirtualAddress();
				m_d3dBoneIndexBufferView.StrideInBytes = sizeof(XMUINT4);
				m_d3dBoneIndexBufferView.SizeInBytes = sizeof(XMUINT4) * m_nVertices;

				nReads = (UINT)::fread(m_pxmf4BoneWeights, sizeof(XMFLOAT4), m_nVertices, pInFile);
				m_pd3dBoneWeightBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf4BoneWeights,
					sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneWeightUploadBuffer);

				m_d3dBoneWeightBufferView.BufferLocation = m_pd3dBoneWeightBuffer->GetGPUVirtualAddress();
				m_d3dBoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
				m_d3dBoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "</SkinningInfo>"))
		{
			break;
		}
	}
}

void CSkinnedMesh::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
}

void CSkinnedMesh::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dcbBindPoseBoneOffsets)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneOffsetsGpuVirtualAddress = m_pd3dcbBindPoseBoneOffsets->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_BONE_OFFSETS, d3dcbBoneOffsetsGpuVirtualAddress); //Skinned Bone Offsets
	}
	 //Skinned Bone Transforms
}

void CSkinnedMesh::ReleaseShaderVariables()
{
	//-----------------------------------------
	if (m_pd3dcbBindPoseBoneOffsets)
	{
		m_pd3dcbBindPoseBoneOffsets->Unmap(0, NULL);
		m_pd3dcbBindPoseBoneOffsets->Release();
	}
	m_pd3dcbBindPoseBoneOffsets = NULL;
	//-----------------------------------------
	if (m_pd3dcbBoneTransforms)
	{
		m_pd3dcbBoneTransforms->Unmap(0, NULL);
		m_pd3dcbBoneTransforms->Release();
	}
	m_pd3dcbBoneTransforms = NULL;
	//-----------------------------------------

}

void CSkinnedMesh::ReleaseUploadBuffers()
{
	CStandardMesh::ReleaseUploadBuffers();
	//-----------------------------------------
	if (m_pd3dBoneIndexUploadBuffer)
		m_pd3dBoneIndexUploadBuffer->Release();

	m_pd3dBoneIndexUploadBuffer = NULL;
	//-----------------------------------------
	if (m_pd3dBoneWeightUploadBuffer) 
		m_pd3dBoneWeightUploadBuffer->Release();

	m_pd3dBoneWeightUploadBuffer = NULL;
	//-----------------------------------------
}

void CSkinnedMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	UpdateShaderVariables(pd3dCommandList);

	const int ViewNum = 7;
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[ViewNum] =
	{ m_MeshInfo.m_d3dPositionBufferView, m_MeshInfo.m_d3dTextureCoord0BufferView, m_d3dNormalBufferView,
		m_d3dTangentBufferView, m_d3dBiTangentBufferView, m_d3dBoneIndexBufferView,m_d3dBoneWeightBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, ViewNum, pVertexBufferViews);

	if ((m_MeshInfo.m_nSubMeshes > 0) && (nSubSet < m_MeshInfo.m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_MeshInfo.m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_MeshInfo.m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CSkinnedMesh::PrepareSkinning(CGameObject* pModelRootObject)
{
}
