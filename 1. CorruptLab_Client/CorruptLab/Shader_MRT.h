#pragma once
#include "stdafx.h"
#include "Shader.h"


struct VS_CB_EYE_CAMERA_ORTHO
{
	XMFLOAT4X4						m_xmf4x4Ortho;
	XMFLOAT4X4						m_xmf4x4OrthoView;
};

class CUI_MiniMap;

class CPostProcessingShader : public CShader
{
protected:
	//---------------------------------------
	CTexture                     * m_pTexture;
	CTexture                     * m_pLightTexture;
	CTexture                     * m_pShadowTexture;
	CTexture                     * m_pItemTex;
	CTexture                     * pMinmapFog1;
	CTexture                     * pMinmapFog2;

	CShader                      * m_pBaseUIShader; 
	CShader                      * m_pEndingShader;
	CShader                      * m_pMinimapFog;
	CShader                      * m_pItemShader;
	
	float                          m_EndingCount = 0.0f;
	int                            m_nMRTSwitch;
	int                            m_nRenderTargetUI;
	CGameObject                 ** m_pRenderTargetUIs ;
	CMRTUI                       * pRenderTargetUI;
					            
	CUI_Root                     * m_pHP;
	CUI_Root                     * m_Radiation;

	CUI_ITem                     * m_pMapOne;
	CUI_ITem                     * m_pMapTwo;
				            
	CUI_MiniMap                  * m_pMinimap;

	CShader                      * m_pRadiationShader;
	CGameObject                 ** m_RadiationLevels;
	CUI_RaditaionLevel           * m_RadiationCount;
					            
	CUI_Root                     * m_StatementUI;
	CUI_Root                     * m_EndingCreditUI;

					            
	CGameObject                 ** m_ppInVentoryBoxs;
	UINT                           nIventoryCount;

	CGameObject                 ** m_ppIndoorScenenumberBox;
	UINT                           nNumberCount;

	CUI_Root                     * m_pIndoorNumberBox;
	CGameObject                 ** m_IndoorNumberCounts;

	CUI_Root                    *  m_pInventoryBox;
	CUI_Root                    * m_IndoorGreeting; 
	float                         m_Alpha = 1;
	CGameObject                 ** m_ppItems;
	CUI_ITem                     * m_pItem ;
					            
	CUI_HP                       * m_PlayerHP;
					            
	ID3D12Resource               * m_pd3dcbvOrthoCamera;
	VS_CB_EYE_CAMERA_ORTHO       * m_pcbMappedOrthoCamera;

	XMFLOAT4X4		               m_xmf4x4OrthoView;
	XMFLOAT4X4                     m_xmf4x4Ortho;

	bool                           m_bPasswordRenderControl = true;

public:

	CPostProcessingShader();
	virtual ~CPostProcessingShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateIndoorPixelShader(ID3DBlob** ppd3dShaderBlob);


	virtual void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets = 1);

	virtual void SetRenderTargets(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		void* pContext = NULL, void* pLightContext = NULL, void* pShadowContext = NULL);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildObjectIndoorScenePassword(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildObjectPlayerStateUICollection(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildObjectMinimap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildObjectEnding(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);


	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void EndingRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int npipelinestate = 0, float fElapsedTime = 0);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int npipelinestate = 0, float fElapsedTime = 0 );

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void UIRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera,  int npipelinestate = 0);
	void UIEndingRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int npipelinestate = 0, float fElapsedTime = 0);
	void GenerateOrthoLHMatrix(float fWidth, float fHeight, float fNearPlaneDistance, float fFarPlaneDistance);
	int GetMRTSwitch() { return m_nMRTSwitch; }
	void SetMRTSwitch(int iunder) { m_nMRTSwitch = iunder; }
	CUI_MiniMap* GetMinimap() { return m_pMinimap; }
	CUI_HP* GetPlayerHP() { return m_PlayerHP; }
};


class CPostProcessingByLaplacianShader : public CPostProcessingShader
{
public:
	CPostProcessingByLaplacianShader();
	virtual ~CPostProcessingByLaplacianShader();

	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

};