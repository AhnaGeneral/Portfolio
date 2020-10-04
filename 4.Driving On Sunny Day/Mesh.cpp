//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Mesh.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
CMeshLoadInfo::~CMeshLoadInfo()
{
	if (m_pxmf3Positions)
		delete[] m_pxmf3Positions;
	if (m_pxmf4Colors)
		delete[] m_pxmf4Colors;
	if (m_pxmf3Normals)
		delete[] m_pxmf3Normals;
	if (m_pnIndices)
		delete[] m_pnIndices;
	if (m_pnSubSetIndices)
		delete[] m_pnSubSetIndices;
	for (int i = 0; i < m_nSubMeshes; i++)
		if (m_ppnSubSetIndices[i])
			delete[] m_ppnSubSetIndices[i];
	if (m_ppnSubSetIndices)
		delete[] m_ppnSubSetIndices;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
CMeshFromFile::CMeshFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CMeshLoadInfo *pMeshInfo)
{
	m_nVertices = pMeshInfo->m_nVertices;
	m_nType = pMeshInfo->m_nType;

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pMeshInfo->m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
	m_nSubMeshes = pMeshInfo->m_nSubMeshes;

	if (m_nSubMeshes > 0)
	{
		m_ppd3dSubSetIndexBuffers = new ID3D12Resource*[m_nSubMeshes];
		m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource*[m_nSubMeshes];
		m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];
		m_pnSubSetIndices = new int[m_nSubMeshes];

		for (int i = 0; i < m_nSubMeshes; i++)
		{
			m_pnSubSetIndices[i] = pMeshInfo->m_pnSubSetIndices[i];
			m_ppd3dSubSetIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pMeshInfo->m_ppnSubSetIndices[i], sizeof(UINT) * m_pnSubSetIndices[i], 
				D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[i]);
			m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
			m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
			m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * pMeshInfo->m_pnSubSetIndices[i];
		}
	}
}

CMeshFromFile::~CMeshFromFile()
{
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	if (m_nSubMeshes > 0)
	{
		for (int i = 0; i < m_nSubMeshes; i++)
		{
			if (m_ppd3dSubSetIndexBuffers[i]) m_ppd3dSubSetIndexBuffers[i]->Release();
		}
		if (m_ppd3dSubSetIndexBuffers) delete[] m_ppd3dSubSetIndexBuffers;
		if (m_pd3dSubSetIndexBufferViews) delete[] m_pd3dSubSetIndexBufferViews;
		if (m_pnSubSetIndices) delete[] m_pnSubSetIndices;
	}
}

void CMeshFromFile::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();

	if (m_pd3dPositionUploadBuffer) m_pd3dPositionUploadBuffer->Release();
	m_pd3dPositionUploadBuffer = NULL;

	if ((m_nSubMeshes > 0) && m_ppd3dSubSetIndexUploadBuffers)
	{
		for (int i = 0; i < m_nSubMeshes; i++)
		{
			if (m_ppd3dSubSetIndexUploadBuffers[i]) m_ppd3dSubSetIndexUploadBuffers[i]->Release();
		}
		if (m_ppd3dSubSetIndexUploadBuffers) delete[] m_ppd3dSubSetIndexUploadBuffers;
		m_ppd3dSubSetIndexUploadBuffers = NULL;
	}
}

void CMeshFromFile::Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else 	{ pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0); }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
CMeshIlluminatedFromFile::CMeshIlluminatedFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CMeshLoadInfo *pMeshInfo) : CMeshFromFile::CMeshFromFile(pd3dDevice, pd3dCommandList, pMeshInfo)
{
	m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pMeshInfo->m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

	m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dTextureBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pMeshInfo->m_pxmf2Textures, sizeof(XMFLOAT2) * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureUploadBuffer);
	//[질문]업로드 버퍼는 뭐하는거에요 ?
	m_d3dTextureBufferView.BufferLocation = m_pd3dTextureBuffer->GetGPUVirtualAddress();
	m_d3dTextureBufferView.StrideInBytes = sizeof(XMFLOAT2);
	m_d3dTextureBufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;

}

CMeshIlluminatedFromFile::~CMeshIlluminatedFromFile()
{
	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
	if (m_pd3dTextureBuffer) m_pd3dTextureBuffer->Release();
}

void CMeshIlluminatedFromFile::ReleaseUploadBuffers()
{
	CMeshFromFile::ReleaseUploadBuffers();

	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer->Release();
	m_pd3dNormalUploadBuffer = NULL;

	if (m_pd3dTextureUploadBuffer) m_pd3dTextureUploadBuffer->Release();
	m_pd3dTextureUploadBuffer = NULL;
}

void CMeshIlluminatedFromFile::Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[3] =
	{ m_d3dPositionBufferView,m_d3dNormalBufferView, m_d3dTextureBufferView };

	pd3dCommandList->IASetVertexBuffers(m_nSlot, 3, pVertexBufferViews);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	BYTE *pHeightMapPixels = new BYTE[m_nWidth * m_nLength];

	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapPixels[x + ((m_nLength - 1 - y)*m_nWidth)] = pHeightMapPixels[x + (y*m_nWidth)];
		}
	}

	if (pHeightMapPixels) delete[] pHeightMapPixels;
}

CHeightMapImage::~CHeightMapImage()
{
	if (m_pHeightMapPixels) delete[] m_pHeightMapPixels;
	m_pHeightMapPixels = NULL;
}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength)) return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return(xmf3Normal);
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER

float CHeightMapImage::GetHeight(float fx, float fz, bool bReverseQuad)
{
	fx = fx / m_xmf3Scale.x;
	fz = fz / m_xmf3Scale.z;
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pHeightMapPixels[x + (z*m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z*m_nWidth)];
	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1)*m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1)*m_nWidth)];
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	if (bReverseQuad)
	{
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight);
}

CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
	int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void *pContext) : CMesh()
{
	//m_nVertices = nWidth * nLength;
	m_nOffset = 0;
	m_nSlot = 0;
	//m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
	m_nVertices = 25;

	CDiffused2TexturedVertex *pVertices = new CDiffused2TexturedVertex[m_nVertices];

	CHeightMapImage *pHeightMapImage = (CHeightMapImage *)pContext;
	int cxHeightMap = pHeightMapImage->GetHeightMapWidth();
	int czHeightMap = pHeightMapImage->GetHeightMapLength();

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	// cxHeightMap = 257-1 
	// scale 8 2 8

	//for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	//{
	//	for (int x = xStart; x < (xStart + nWidth); x++, i++)
	//	{
	//		fHeight = OnGetHeight(x, z, pContext);
	//		pVertices[i].m_xmf3Position = XMFLOAT3((x*m_xmf3Scale.x), fHeight, (z*m_xmf3Scale.z));
	//		pVertices[i].m_xmf4Diffuse = Vector4::Add(OnGetColor(x, z, pContext), xmf4Color);
	//		pVertices[i].m_xmf2TexCoord0 = XMFLOAT2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) / float(czHeightMap - 1));
	//		pVertices[i].m_xmf2TexCoord1 = XMFLOAT2(float(x) / float(m_xmf3Scale.x*0.5f), float(z) / float(m_xmf3Scale.z*0.5f));
	//		if (fHeight < fMinHeight) fMinHeight = fHeight;
	//		if (fHeight > fMaxHeight) fMaxHeight = fHeight;
	//	}
	//}

	for (int i = 0, z = (zStart + nLength - 1); z >= zStart; z -= 2)
	{
		for (int x = xStart; x < (xStart + nWidth); x += 2, i++)
		{
			fHeight = OnGetHeight(x, z, pContext);
			pVertices[i].m_xmf3Position = XMFLOAT3((x * m_xmf3Scale.x), fHeight, (z * m_xmf3Scale.z));
			pVertices[i].m_xmf4Diffuse = Vector4::Add(OnGetColor(x, z, pContext), xmf4Color);
			pVertices[i].m_xmf2TexCoord0 = XMFLOAT2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) / float(czHeightMap - 1));
			pVertices[i].m_xmf2TexCoord1 = XMFLOAT2(float(x) / float(m_xmf3Scale.x * 0.5f), float(z) / float(m_xmf3Scale.z * 0.5f));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, sizeof(CDiffused2TexturedVertex) * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = sizeof(CDiffused2TexturedVertex);
	m_d3dVertexBufferView.SizeInBytes = sizeof(CDiffused2TexturedVertex) * m_nVertices;

	delete[] pVertices;

	//m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
	//UINT *pnIndices = new UINT[m_nIndices];

	//for (int j = 0, z = 0; z < nLength - 1; z++)
	//{
	//	if ((z % 2) == 0)
	//	{
	//		for (int x = 0; x < nWidth; x++)
	//		{
	//			if ((x == 0) && (z > 0)) pnIndices[j++] = (UINT)(x + (z * nWidth));
	//			pnIndices[j++] = (UINT)(x + (z * nWidth));
	//			pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
	//		}
	//	}
	//	else
	//	{
	//		for (int x = nWidth - 1; x >= 0; x--)
	//		{
	//			if (x == (nWidth - 1)) pnIndices[j++] = (UINT)(x + (z * nWidth));
	//			pnIndices[j++] = (UINT)(x + (z * nWidth));
	//			pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
	//		}
	//	}
	//}

	//m_pd3dIndexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices,
	//	D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	//m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	//m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	//m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	//delete[] pnIndices;
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
		if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
		if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
		if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
		if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void *pContext)
{
	CHeightMapImage *pHeightMapImage = (CHeightMapImage *)pContext;
	BYTE *pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetHeightMapWidth();
	float fHeight = pHeightMapPixels[x + (z*nWidth)] * xmf3Scale.y;
	return(fHeight);
}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void *pContext)
{
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);
	CHeightMapImage *pHeightMapImage = (CHeightMapImage *)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);
	float fScale = Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1), xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;
	if (fScale > 1.0f) fScale = 1.0f;
	if (fScale < 0.25f) fScale = 0.25f;
	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);
	return(xmf4Color);
}

void CHeightMapGridMesh::ReleaseUploadBuffers()
{
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
	m_pd3dIndexUploadBuffer = NULL;
}

void CHeightMapGridMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList, int nSubSet)
{
	
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	//if (m_pd3dIndexBuffer)
	//{
	//	pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
	//	pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	//}
	
	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	
}

CWaterSquare::CWaterSquare(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) 
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 2.0f, fy = fHeight * 2.0f, fz = fDepth * 2.0f; // 200.f , 200.f , 200.f 

	CTexturedVertex pVertices[6];
	int i = 0;

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(4.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(4.0f, 4.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(4.0f, 4.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 4.0f));


	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, sizeof(CTexturedVertex) * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = sizeof(CTexturedVertex);
	m_d3dVertexBufferView.SizeInBytes = sizeof(CTexturedVertex) * m_nVertices;
}

CWaterSquare::~CWaterSquare()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
}

void CWaterSquare::ReleaseUploadBuffers()
{
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

}

void CWaterSquare::Render(ID3D12GraphicsCommandList * pd3dCommandList, int nSubSet)
{

	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	//if (m_pd3dIndexBuffer)
	//{
	//	pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
	//	pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	//}

		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	

}

CBillboardRectMesh::CBillboardRectMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList,
	float fWidth, float fHeight, float fDepth, float fxPosition, float fyPosition, float fzPosition)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	CTexturedVertex pVertices[6];
	float fx = (fWidth*0.5f) + fxPosition;
	float fy = (fHeight*0.5f) + fyPosition;
	float fz = (fDepth*0.5f) + fzPosition;

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

	m_pd3dVertexBuffer = CreateBufferResource
	(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CBillboardRectMesh::~CBillboardRectMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
}

void CBillboardRectMesh::ReleaseUploadBuffers()
{
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
}

void CBillboardRectMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}


CStartScreenRectMesh::CStartScreenRectMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList,
	float fWidth, float fHeight, float fDepth, float fxPosition, float fyPosition, float fzPosition)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	CTexturedVertex pVertices[6];
	float fx = (fWidth*0.5f) + fxPosition;
	float fy = (fHeight*0.5f) + fyPosition;
	float fz = (fDepth*0.5f) + fzPosition;

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

	m_pd3dVertexBuffer = CreateBufferResource
	(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CStartScreenRectMesh::~CStartScreenRectMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
}

void CStartScreenRectMesh::ReleaseUploadBuffers()
{
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
}

void CStartScreenRectMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

CRawFormatImage::CRawFormatImage(const LPCTSTR pFileName, int nWidth, int nLength, bool bFlipY)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	BYTE* pRawImagePixels = new BYTE[m_nWidth * m_nLength];

	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pRawImagePixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	if (bFlipY)
	{
		m_pRawImagePixels = new BYTE[m_nWidth * m_nLength];
		for (int z = 0; z < m_nLength; z++)
		{
			for (int x = 0; x < m_nWidth; x++)
			{
				m_pRawImagePixels[x + ((m_nLength - 1 - z) * m_nWidth)] = pRawImagePixels[x + (z * m_nWidth)];
			}
		}

		if (pRawImagePixels) delete[] pRawImagePixels;
	}
	else
	{
		m_pRawImagePixels = pRawImagePixels;
	}
}

CRawFormatImage::~CRawFormatImage()
{
	if (m_pRawImagePixels) delete[] m_pRawImagePixels;
	m_pRawImagePixels = NULL;
}
