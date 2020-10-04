#pragma once 
#include "Object.h"


struct GS_BILLBOARD_INSTANCE
{
	XMFLOAT3 m_xmf3Position;
	XMFLOAT2 m_xmf2Size;
};


class CMRTUI : public CGameObject
{
private:
	UINT m_nobjectID = 0;

public:
	CMRTUI() {}
	CMRTUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMRTUI();
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);
	virtual void InterLinkShaderTexture(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* Texture = NULL) {}
	virtual void ReleaseShaderVariables();
	//virtual void ReleaseUploadBuffers();
	void SetObjectID(UINT objectID);
	virtual UINT GetObjectID() { return m_nobjectID; }
	void Set2DPosition(float x, float y);
};


class CUI_MiniMap : public CMRTUI
{
public:
	CUI_MiniMap();
	CUI_MiniMap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual ~CUI_MiniMap() {}

	virtual void InterLinkShaderTexture(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* Texture = NULL);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void ReleaseShaderVariables();

	//virtual void ReleaseUploadBuffers();
	void SetPlayerPosition(XMFLOAT3* pos) { m_PlayerPosition = pos; }

private:
	ID3D12Resource* m_pd3dcbPlayerPosition = NULL;
	XMFLOAT3* m_PlayerPosition = NULL;
	XMFLOAT3* m_pcbPlayerPosition = NULL;
};

class CUI_Root : public CMRTUI
{
public:
	CUI_Root();
	CUI_Root(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CUI_Root() {}

	virtual void InterLinkShaderTexture(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pShader = NULL, void* pTexture = NULL);

	void SetAlpha(float* AlphaDegree) { m_Alpha = AlphaDegree; }
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void ReleaseShaderVariables();

private:
	ID3D12Resource* m_pd3dcbAlpha = NULL;
	float               * m_Alpha = NULL;
	float            * m_pcbAlpha = NULL;
};

struct CB_ITEM
{
	XMFLOAT4 m_f4iTemCount = { 0.0f,0.0f,0.0f,0.0f };
	int    m_iTemReact = 0; 
	float  m_fTIme = 0;
	int    m_Control = 0;
};

class CUI_ITem : public CMRTUI
{
protected:
	UINT     m_nobjectID = 0 ;
	CB_ITEM  m_ItemReact;

public:
	CUI_ITem();
	CUI_ITem(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CUI_ITem() {}

	virtual void InterLinkShaderTexture(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* Texture = NULL);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void ReleaseShaderVariables();

	void SetObjectID(UINT objectID) { m_nobjectID = objectID; }
	
	virtual UINT GetObjectID() { return m_nobjectID; }
	
	void SetItemReact(int React) { m_ItemReact.m_iTemReact = React;}
	
	int GetFogControl() { return m_ItemReact.m_Control; }

	void SetItemCount(XMFLOAT4 itemCount);
	///void SetItemElapsedTime(){}
	
protected:
	ID3D12Resource* m_pd3dcbItemReact = NULL;
	CB_ITEM* m_pcbItemReact = NULL;
};


class CUI_HP : public CMRTUI
{
public:
	CUI_HP();
	CUI_HP(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual ~CUI_HP() {}

	virtual void InterLinkShaderTexture(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* Texture = NULL);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void ReleaseShaderVariables();


	void SetPlayerHP(float* playerHPRemaining) { m_PlayerHP = playerHPRemaining; }
	void SetObjectID(UINT objectID) { m_nobjectID = objectID; }
	virtual UINT GetObjectID() { return m_nobjectID; }
	 
private:
	UINT m_nobjectID = 0 ;

	ID3D12Resource* m_pd3dcbPlayerHP = NULL;
	float* m_PlayerHP = NULL;
	float* m_pcbPlayerHP = NULL;

};

class CUI_RaditaionLevel : public CMRTUI
{
public:
	CUI_RaditaionLevel();
	CUI_RaditaionLevel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CUI_RaditaionLevel() {}

	virtual void InterLinkShaderTexture(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pShader = NULL, void* pTexture = NULL);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void ReleaseShaderVariables();
	void SetRadiationNumber(float num);

private:
	ID3D12Resource* m_pd3dcbRadiationLevel = NULL;
	float m_RadiationNumber = 0 ;
	float* m_pcbRadiationNum = NULL;
};

class CUI_MonsterHP : public CMRTUI
{
public:
	CUI_MonsterHP();
	CUI_MonsterHP(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual ~CUI_MonsterHP() {}

	virtual void SetInstanceInfo( XMFLOAT2 Scale, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);


	void SetObjectID(UINT objectID) { m_nobjectID = objectID; }
	virtual UINT GetObjectID() { return m_nobjectID; }

	ID3D12Resource* m_pd3dcbMonsterHP = NULL;
	int* m_MonsterHP = NULL;
	int* m_pcbMonsterHP = NULL;

private:
	UINT m_nobjectID = 0;
	GS_BILLBOARD_INSTANCE InstanceInfo;

	ID3D12Resource* m_pd3dPositionBuffer = NULL;
	ID3D12Resource* m_pd3dPositionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;
};
