#include "Object_Effect.h"

CObject_Effect::CObject_Effect()
{
}

CObject_Effect::CObject_Effect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 position, CShader* pShader)
{
	m_position = new XMFLOAT3;
	*m_position = position;
	m_pd3dPositionBuffer =
		::CreateBufferResource(pd3dDevice, pd3dCommandList, &m_position, sizeof(XMFLOAT3),
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	
	m_pd3dPositionBuffer->Map(0, NULL, (void**)&m_position);
	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CObject_Effect::~CObject_Effect()
{

}

void CObject_Effect::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// GameObject[게임 오브젝트 콘스탄트버퍼] -------------------------------------------------------------
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);

}

void CObject_Effect::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CObject_Effect::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	if (m_pd3dPositionBuffer)
	{
		//	m_pd3dPositionBuffer->Unmap(0, NULL);
		m_pd3dPositionUploadBuffer->Release();
		m_pd3dPositionBuffer->Release();
	}
}

void CObject_Effect::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

	UINT getobjectID = 0;
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 4, &getobjectID, 16);
}

void CObject_Effect::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[1] = { m_d3dPositionBufferView };
	pd3dCommandList->IASetVertexBuffers(0, _countof(pVertexBufferViews), pVertexBufferViews);
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	pd3dCommandList->DrawInstanced(1, 1, 0, 0);
}

void CObject_Effect::UpdatePosition(XMFLOAT3 _NewPos)
{
	*m_position = _NewPos;
	memcpy(m_position, (void*)&_NewPos, (sizeof(XMFLOAT3)));
}
