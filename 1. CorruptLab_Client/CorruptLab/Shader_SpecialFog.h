#pragma once
#include"Shader_BillBoard.h"

class CShader_SpecialFog :public CSoftParticleShader
{
private:
	CObjectNosie** m_pOneStageFog;
	CObjectNosie** m_pTwoStageFog;

	ID3D12Resource* m_pd3dAngle = NULL;
	XMFLOAT3* Angle = NULL;

	int	m_nOneFog;
	int	m_nTwoFog;

public:
	CShader_SpecialFog();
	~CShader_SpecialFog();


	virtual D3D12_BLEND_DESC         CreateBlendState();
	virtual D3D12_INPUT_LAYOUT_DESC  CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE    CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE    CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE    CreateGeometryShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	D3D12_SHADER_BYTECODE    CreateFogVertexShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE    CreateFogPixelShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE    CreateFogGeometryShader(ID3DBlob** ppd3dShaderBlob);

	void CreateNoiseTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual D3D12_RASTERIZER_DESC    CreateRasterizerState();
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets = 1);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, CHeightMapTerrain* pTerrain = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

};