#pragma once
#include "Object.h"

class CCollisionBox {

public:
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);

	BoundingOrientedBox boundingBox;
	XMFLOAT3 m_Center;
	XMFLOAT3 m_Extents;
	XMFLOAT4 m_Orientation;
	int		 m_iBoneIndex;

	XMFLOAT4X4						m_xmf4x4World;
	CGameObject* m_pParent = NULL;

	ID3D12Resource* m_pd3dCollisionBuffer = NULL;
	ID3D12Resource* m_pd3dCollisionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		 m_d3dCollisionBufferView;


};
