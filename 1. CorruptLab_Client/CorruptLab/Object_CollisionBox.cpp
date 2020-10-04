#include "Object_CollisionBox.h"
#include "Shader_CollisionBox.h"

void CCollisionBox::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_xmf4x4World = m_pParent->m_xmf4x4World;
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

}

void CCollisionBox::BuildBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{

	GS_COLLISION_BOX_INFO info;
	info.m_xmf3Center = m_Center;
	info.m_xmf3Extent = m_Extents;
	info.m_xmf4Orientation = m_Orientation;
	m_pd3dCollisionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &info,
		sizeof(GS_COLLISION_BOX_INFO), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dCollisionUploadBuffer);

	m_d3dCollisionBufferView.BufferLocation = m_pd3dCollisionBuffer->GetGPUVirtualAddress();
	m_d3dCollisionBufferView.StrideInBytes = sizeof(GS_COLLISION_BOX_INFO);
	m_d3dCollisionBufferView.SizeInBytes = sizeof(GS_COLLISION_BOX_INFO);
}

void CCollisionBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	UpdateShaderVariables(pd3dCommandList);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[1] = { m_d3dCollisionBufferView };
	pd3dCommandList->IASetVertexBuffers(0, _countof(pVertexBufferViews), pVertexBufferViews);
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	pd3dCommandList->DrawInstanced(1, 1, 0, 0);
}

