#include "stdafx.h"
#include "Mesh_SkyBox.h"

// CSkyBoxMesh-----------------------------------------------------------------------------------
CSkyBoxMesh::CSkyBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_MeshInfo.m_pxmf3Positions = new XMFLOAT3[m_nVertices];

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	// Front Quad (quads point inward)
	m_MeshInfo.m_pxmf3Positions[0] = XMFLOAT3(-fx, +fx, +fx);
	m_MeshInfo.m_pxmf3Positions[1] = XMFLOAT3(+fx, +fx, +fx);
	m_MeshInfo.m_pxmf3Positions[2] = XMFLOAT3(-fx, -fx, +fx);
	m_MeshInfo.m_pxmf3Positions[3] = XMFLOAT3(-fx, -fx, +fx);
	m_MeshInfo.m_pxmf3Positions[4] = XMFLOAT3(+fx, +fx, +fx);
	m_MeshInfo.m_pxmf3Positions[5] = XMFLOAT3(+fx, -fx, +fx);
	// Back Quad										
	m_MeshInfo.m_pxmf3Positions[6] = XMFLOAT3(+fx, +fx, -fx);
	m_MeshInfo.m_pxmf3Positions[7] = XMFLOAT3(-fx, +fx, -fx);
	m_MeshInfo.m_pxmf3Positions[8] = XMFLOAT3(+fx, -fx, -fx);
	m_MeshInfo.m_pxmf3Positions[9] = XMFLOAT3(+fx, -fx, -fx);
	m_MeshInfo.m_pxmf3Positions[10] = XMFLOAT3(-fx, +fx, -fx);
	m_MeshInfo.m_pxmf3Positions[11] = XMFLOAT3(-fx, -fx, -fx);
	// Left Quad										
	m_MeshInfo.m_pxmf3Positions[12] = XMFLOAT3(-fx, +fx, -fx);
	m_MeshInfo.m_pxmf3Positions[13] = XMFLOAT3(-fx, +fx, +fx);
	m_MeshInfo.m_pxmf3Positions[14] = XMFLOAT3(-fx, -fx, -fx);
	m_MeshInfo.m_pxmf3Positions[15] = XMFLOAT3(-fx, -fx, -fx);
	m_MeshInfo.m_pxmf3Positions[16] = XMFLOAT3(-fx, +fx, +fx);
	m_MeshInfo.m_pxmf3Positions[17] = XMFLOAT3(-fx, -fx, +fx);
	// Right Quad										
	m_MeshInfo.m_pxmf3Positions[18] = XMFLOAT3(+fx, +fx, +fx);
	m_MeshInfo.m_pxmf3Positions[19] = XMFLOAT3(+fx, +fx, -fx);
	m_MeshInfo.m_pxmf3Positions[20] = XMFLOAT3(+fx, -fx, +fx);
	m_MeshInfo.m_pxmf3Positions[21] = XMFLOAT3(+fx, -fx, +fx);
	m_MeshInfo.m_pxmf3Positions[22] = XMFLOAT3(+fx, +fx, -fx);
	m_MeshInfo.m_pxmf3Positions[23] = XMFLOAT3(+fx, -fx, -fx);
	// Top Quad											
	m_MeshInfo.m_pxmf3Positions[24] = XMFLOAT3(-fx, +fx, -fx);
	m_MeshInfo.m_pxmf3Positions[25] = XMFLOAT3(+fx, +fx, -fx);
	m_MeshInfo.m_pxmf3Positions[26] = XMFLOAT3(-fx, +fx, +fx);
	m_MeshInfo.m_pxmf3Positions[27] = XMFLOAT3(-fx, +fx, +fx);
	m_MeshInfo.m_pxmf3Positions[28] = XMFLOAT3(+fx, +fx, -fx);
	m_MeshInfo.m_pxmf3Positions[29] = XMFLOAT3(+fx, +fx, +fx);
	// Bottom Quad										
	m_MeshInfo.m_pxmf3Positions[30] = XMFLOAT3(-fx, -fx, +fx);
	m_MeshInfo.m_pxmf3Positions[31] = XMFLOAT3(+fx, -fx, +fx);
	m_MeshInfo.m_pxmf3Positions[32] = XMFLOAT3(-fx, -fx, -fx);
	m_MeshInfo.m_pxmf3Positions[33] = XMFLOAT3(-fx, -fx, -fx);
	m_MeshInfo.m_pxmf3Positions[34] = XMFLOAT3(+fx, -fx, +fx);
	m_MeshInfo.m_pxmf3Positions[35] = XMFLOAT3(+fx, -fx, -fx);

	m_MeshInfo.m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_MeshInfo.m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_MeshInfo.m_pd3dPositionUploadBuffer);

	m_MeshInfo.m_d3dPositionBufferView.BufferLocation = m_MeshInfo.m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_MeshInfo.m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_MeshInfo.m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
}
CSkyBoxMesh::~CSkyBoxMesh()
{
	//CMesh::~CMesh();
	CMesh::ReleaseUploadBuffers();
}

void CSkyBoxMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[1] = { m_MeshInfo.m_d3dPositionBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, pVertexBufferViews);

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