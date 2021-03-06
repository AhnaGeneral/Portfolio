//-----------------------------------------------------------------------------
// File: CMesh.cpp
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "Mesh.h"
#include "Object.h"

// CMesh-----------------------------------------------------------------------------------------
CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }

CMesh::~CMesh()
{
	if (m_MeshInfo.m_pd3dPositionBuffer)  m_MeshInfo.m_pd3dPositionBuffer->Release();
	if (m_MeshInfo.m_pxmf3Positions) delete m_MeshInfo.m_pxmf3Positions;

	if (m_MeshInfo.m_pd3dColorBuffer) m_MeshInfo.m_pd3dColorBuffer->Release();
	if (m_MeshInfo.m_pxmf4Colors) delete m_MeshInfo.m_pxmf4Colors;

	if (m_MeshInfo.m_pd3dTextureCoord0Buffer) m_MeshInfo.m_pd3dTextureCoord0Buffer->Release();
	if (m_MeshInfo.m_pxmf2TextureCoords0) delete m_MeshInfo.m_pxmf2TextureCoords0;

	if (m_MeshInfo.m_pd3dTextureCoord1Buffer) m_MeshInfo.m_pd3dTextureCoord1Buffer->Release();
	if (m_MeshInfo.m_pxmf2TextureCoords1) delete m_MeshInfo.m_pxmf2TextureCoords1;


	if (m_MeshInfo.m_nSubMeshes > 0)
	{
		for (int i = 0; i < m_MeshInfo.m_nSubMeshes; ++i)
		{
			if (m_MeshInfo.m_ppd3dSubSetIndexBuffers[i])
				m_MeshInfo.m_ppd3dSubSetIndexBuffers[i]->Release();
			if (m_MeshInfo.m_ppnSubSetIndices[i]) delete[] m_MeshInfo.m_ppnSubSetIndices[i];
		}
		if (m_MeshInfo.m_ppd3dSubSetIndexBuffers) delete[] m_MeshInfo.m_ppd3dSubSetIndexBuffers;
		if (m_MeshInfo.m_pd3dSubSetIndexBufferViews) delete[] m_MeshInfo.m_pd3dSubSetIndexBufferViews;

		if (m_MeshInfo.m_pnSubSetIndices) delete[]  m_MeshInfo.m_pnSubSetIndices;
		if (m_MeshInfo.m_ppnSubSetIndices) delete[] m_MeshInfo.m_ppnSubSetIndices;
	}
}

void CMesh::ReleaseUploadBuffers()
{
	if (m_MeshInfo.m_pd3dPositionUploadBuffer) m_MeshInfo.m_pd3dPositionUploadBuffer->Release();
	if (m_MeshInfo.m_pd3dColorUploadBuffer) m_MeshInfo.m_pd3dColorUploadBuffer->Release();
	if (m_MeshInfo.m_pd3dTextureCoord0UploadBuffer) m_MeshInfo.m_pd3dTextureCoord0UploadBuffer->Release();
	if (m_MeshInfo.m_pd3dTextureCoord1UploadBuffer) m_MeshInfo.m_pd3dTextureCoord1UploadBuffer->Release();

	m_MeshInfo.m_pd3dPositionUploadBuffer = NULL;
	m_MeshInfo.m_pd3dColorUploadBuffer = NULL;
	m_MeshInfo.m_pd3dTextureCoord0UploadBuffer = NULL;
	m_MeshInfo.m_pd3dTextureCoord1UploadBuffer = NULL;

	if ((m_MeshInfo.m_nSubMeshes > 0) && m_MeshInfo.m_ppd3dSubSetIndexUploadBuffers)
	{
		for (int i = 0; i < m_MeshInfo.m_nSubMeshes; i++)
		{
			if (m_MeshInfo.m_ppd3dSubSetIndexUploadBuffers[i])
				m_MeshInfo.m_ppd3dSubSetIndexUploadBuffers[i]->Release();
		}
		if (m_MeshInfo.m_ppd3dSubSetIndexUploadBuffers)
			delete[] m_MeshInfo.m_ppd3dSubSetIndexUploadBuffers;
		m_MeshInfo.m_ppd3dSubSetIndexUploadBuffers = NULL;
	}
};

// CStandardMesh----------------------------------------------------------------------------------
CStandardMesh::CStandardMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CStandardMesh::~CStandardMesh()
{
	if (m_pd3dNormalBuffer)m_pd3dNormalBuffer->Release();
	if (m_pxmf3Normals) delete m_pxmf3Normals;

	if (m_pd3dTangentBuffer)m_pd3dTangentBuffer->Release();
	if (m_pxmf3Tangents) delete m_pxmf3Tangents;

	if (m_pd3dBiTangentBuffer)m_pd3dBiTangentBuffer->Release();
	if (m_pxmf3BiTangents) delete m_pxmf3BiTangents;
}

void CStandardMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();
	
	if (m_pd3dTangentUploadBuffer) m_pd3dTangentUploadBuffer->Release();
	m_pd3dTangentUploadBuffer = NULL;

	if (m_pd3dBiTangentUploadBuffer) m_pd3dBiTangentUploadBuffer->Release();
	m_pd3dBiTangentUploadBuffer = NULL;

	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer->Release();
	m_pd3dNormalUploadBuffer = NULL;


}

void CStandardMesh::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nPositions = 0, nColors = 0, nNormals = 0, nIndices = 0,
		nSubMeshes = 0, nSubIndices = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0;

	m_nVertices = ::ReadIntegerFromFile(pInFile);

	::ReadStringFromFile(pInFile, m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&(m_xmf3AABBCenter), sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&(m_xmf3AABBExtents), sizeof(XMFLOAT3), 1, pInFile);
		}

		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nPositions = ::ReadIntegerFromFile(pInFile);
			if (nPositions > 0)
			{
				m_nType |= VERTEXT_POSITION;
				m_MeshInfo.m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(m_MeshInfo.m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, pInFile);

				m_MeshInfo.m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_MeshInfo.m_pxmf3Positions,
					sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_MeshInfo.m_pd3dPositionUploadBuffer);

				m_MeshInfo.m_d3dPositionBufferView.BufferLocation = m_MeshInfo.m_pd3dPositionBuffer->GetGPUVirtualAddress();
				m_MeshInfo.m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_MeshInfo.m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}

		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nColors = ::ReadIntegerFromFile(pInFile);
			if (nColors > 0)
			{
				m_nType |= VERTEXT_COLOR;
				m_MeshInfo.m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(m_MeshInfo.m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}

		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nNormals = ::ReadIntegerFromFile(pInFile);
			if (nNormals > 0)
			{
				m_nType |= VERTEXT_NORMAL;
				m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);

				m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals,
					sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

				m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
				m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}

		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{
				m_nType |= VERTEXT_TANGENT;
				m_pxmf3Tangents = new XMFLOAT3[nTangents];
				nReads = (UINT)::fread(m_pxmf3Tangents, sizeof(XMFLOAT3), nTangents, pInFile);

				m_pd3dTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Tangents,
					sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTangentUploadBuffer);

				m_d3dTangentBufferView.BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
				m_d3dTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}

		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, pInFile);
			if (nBiTangents > 0)
			{
				m_pxmf3BiTangents = new XMFLOAT3[nBiTangents];
				nReads = (UINT)::fread(m_pxmf3BiTangents, sizeof(XMFLOAT3), nBiTangents, pInFile);

				m_pd3dBiTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3BiTangents,
					sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBiTangentUploadBuffer);

				m_d3dBiTangentBufferView.BufferLocation = m_pd3dBiTangentBuffer->GetGPUVirtualAddress();
				m_d3dBiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dBiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}

		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;
				m_MeshInfo.m_pxmf2TextureCoords0 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_MeshInfo.m_pxmf2TextureCoords0, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_MeshInfo.m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_MeshInfo.m_pxmf2TextureCoords0,
					sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_MeshInfo.m_pd3dTextureCoord0UploadBuffer);

				m_MeshInfo.m_d3dTextureCoord0BufferView.BufferLocation = m_MeshInfo.m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
				m_MeshInfo.m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_MeshInfo.m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}
		//TexCoords
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD1;
				m_MeshInfo.m_pxmf2TextureCoords1 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_MeshInfo.m_pxmf2TextureCoords1, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_MeshInfo.m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_MeshInfo.m_pxmf2TextureCoords1,
					sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_MeshInfo.m_pd3dTextureCoord1UploadBuffer);

				m_MeshInfo.m_d3dTextureCoord1BufferView.BufferLocation = m_MeshInfo.m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
				m_MeshInfo.m_d3dTextureCoord1BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_MeshInfo.m_d3dTextureCoord1BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}

		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			m_MeshInfo.m_nSubMeshes = ::ReadIntegerFromFile(pInFile);
			if (m_MeshInfo.m_nSubMeshes > 0)
			{
				m_MeshInfo.m_pnSubSetIndices = new int[m_MeshInfo.m_nSubMeshes];
				m_MeshInfo.m_ppnSubSetIndices = new UINT * [m_MeshInfo.m_nSubMeshes];

				m_MeshInfo.m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_MeshInfo.m_nSubMeshes];
				m_MeshInfo.m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_MeshInfo.m_nSubMeshes];
				m_MeshInfo.m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_MeshInfo.m_nSubMeshes];

				for (int i = 0; i < m_MeshInfo.m_nSubMeshes; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);

					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, pInFile); //i
						nReads = (UINT)::fread(&(m_MeshInfo.m_pnSubSetIndices[i]), sizeof(int), 1, pInFile);

						if (m_MeshInfo.m_pnSubSetIndices[i] > 0)
						{
							m_MeshInfo.m_ppnSubSetIndices[i] = new UINT[m_MeshInfo.m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(m_MeshInfo.m_ppnSubSetIndices[i], sizeof(UINT), m_MeshInfo.m_pnSubSetIndices[i], pInFile);

							m_MeshInfo.m_ppd3dSubSetIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_MeshInfo.m_ppnSubSetIndices[i],
								sizeof(UINT) * m_MeshInfo.m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_MeshInfo.m_ppd3dSubSetIndexUploadBuffers[i]);

							m_MeshInfo.m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_MeshInfo.m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
							m_MeshInfo.m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
							m_MeshInfo.m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_MeshInfo.m_pnSubSetIndices[i];
						}
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
}

void CStandardMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	const int ViewNum = 5;
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[ViewNum] =
	{ m_MeshInfo.m_d3dPositionBufferView, m_MeshInfo.m_d3dTextureCoord0BufferView, m_d3dNormalBufferView, m_d3dTangentBufferView, m_d3dBiTangentBufferView };
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



CTriangleRect::CTriangleRect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	 float fWidth, float fHeight, float fDepth, float fzPosition)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	CTexturedVertex pVertices[6];
	float fx = (fWidth * 0.5f);
	float fy = (fHeight * 0.5f);
	float fz = (fDepth * 0.5f);

	if (fDepth == 0.0f)
	{
		if (fzPosition > 0.0f)
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
		}
	}
	if (fWidth == 0.0f)
	{
		if (fzPosition < 0.0f)
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
		}
	}


	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CTriangleRect::~CTriangleRect()
{
}

void CTriangleRect::ReleaseUploadBuffers()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();

	CMesh::ReleaseUploadBuffers();
}

void CTriangleRect::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}
