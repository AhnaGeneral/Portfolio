#pragma once
#include "Shader.h"
#include "Object_Effect.h"

struct CB_SPTEFFECTTYPE
{
	int row; 
	int col; 
	float LoopTime;
	float LifeTime;
};

class CShader_Effect : public CShader 
{
protected :
	CTexture* m_pEffectTexture = NULL; 

	CObject_Effect* m_pLight01obj = NULL;
	CObject_Effect* m_pSPT_Wave02obj = NULL;
	CObject_Effect* m_pSPT_Tiping = NULL;


	ID3D12Resource* m_pd3dcbEffectBuffer = NULL;
	CB_SPTEFFECTTYPE* m_pcbMappedEffectBuffers = NULL;
	CB_SPTEFFECTTYPE  m_spt;
	float             m_SPTTimeSpeed = 1.0f; 
	//float m_fFrameTime = 0.0f;
	


public :
	CShader_Effect(); 
	virtual ~CShader_Effect(); 

	virtual D3D12_BLEND_DESC         CreateBlendState();
	virtual D3D12_INPUT_LAYOUT_DESC  CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE    CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE    CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE    CreateGeometryShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_RASTERIZER_DESC    CreateRasterizerState();

	D3D12_SHADER_BYTECODE    CreateSPRVertexShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE    CreateSPRPixelShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE    CreateSPRXZGeometryShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE    CreateSPRYZGeometryShader(ID3DBlob** ppd3dShaderBlob);



	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets = 1);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 _pos, bool GreenZoon = true, float _SPTTimeSpeed =1.0f);

	void UpdatePosition(XMFLOAT3 _pos);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool GreenZoon = true);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

};

