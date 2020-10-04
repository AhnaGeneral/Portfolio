#pragma once
#include "stdafx.h"
#include "Shader.h"


struct LIGHT
{
	XMFLOAT4	      m_xmf4Ambient;
	XMFLOAT4	      m_xmf4Diffuse;
	XMFLOAT4	      m_xmf4Specular;
	XMFLOAT3	      m_xmf3Position;
	float 		      m_fFalloff;
	XMFLOAT3	      m_xmf3Direction;
	float 		      m_fTheta; //cos(m_fTheta)
	XMFLOAT3	      m_xmf3Attenuation;
	float		      m_fPhi; //cos(m_fPhi)
	bool		      m_bEnable;
	int			      m_nType;
	float		      m_fRange;
	float		      padding;
};

struct LIGHTS
{
	XMFLOAT4	      m_xmf4GlobalAmbient;
	LIGHT		      m_pLights[MAX_LIGHTS];
};

struct MATERIAL
{
	XMFLOAT4	      m_xmf4Ambient;
	XMFLOAT4	      m_xmf4Diffuse;
	XMFLOAT4	      m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4	      m_xmf4Emissive;
};

struct MATERIALS
{
	MATERIAL	     m_pReflections[MAX_MATERIALS];
};

class CLightTarget : public CShader
{
public:
	CLightTarget();
	virtual ~CLightTarget();

	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets = 1);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	void OutdoorRender(ID3D12GraphicsCommandList* pd3dCommandList,CCamera* pCamera, int nPipelineState = 0);
    void IndoorRender(ID3D12GraphicsCommandList* pd3dCommandList,CCamera* pCamera, int nPipelineState = 0);
	void SetPlayer(CPlayer* Player) { m_pPlayer = Player; }
	void ChangeLights();
	void ChangeLights2();
	void TurnOnLabatoryLight();
	void BuildLightsAndMaterials();

protected:

	CTexture            * m_pTextures;
	CPlayer             * m_pPlayer; 
	MATERIALS           * m_pMaterials;
	LIGHTS              * m_pLights;

	ID3D12Resource      * m_pd3dcbLights;
	LIGHTS              * m_pcbMappedLights ;

	ID3D12Resource      * m_pd3dcbMaterials ;
	MATERIAL            * m_pcbMappedMaterials;

	float                 m_fTime = 0.0f; 
	bool                  m_bTimeControl = true;
	bool                  m_IndoorScene = false; 

};

