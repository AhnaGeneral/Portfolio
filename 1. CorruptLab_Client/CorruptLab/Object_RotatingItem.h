#pragma once
#include "Object_UI.h"
class CRotatingItem :public CUI_ITem
{
public:
	CRotatingItem() {}
	CRotatingItem(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);
	//virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual ~CRotatingItem();

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

public:
	UINT    m_nobjectID = 0;
	bool	m_bAnimate = false;
	float	m_fElapsedTIme = 0.0f;
	int		m_nItemType = 0;

	CTexture* m_pItemTextures[4]; // 알약, 지도, 손전등, 

};


class CPasswordInDesk :public CRotatingItem
{
public:
	CPasswordInDesk() {}
	CPasswordInDesk(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
	//virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	 ~CPasswordInDesk() {}


};
