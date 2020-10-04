#pragma once
#include "Shader.h"
#include "Object_BillBoard.h"


class CSkyBoxShader : public CShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets = 1);
};

class CTerrain;
class CObjectNosie;

class CSoftParticleShader :public CShader
{
protected:
	CTexture                       * m_pSceneDepthTextures;
	CTexture                       * m_pFireNoiseTextures;
	CTexture                       * m_pFogNoiseTextures;

	CObjectNosie			      ** m_pFireObjects;
	int			                     m_nFire;

	CObjectNosie                 **	 m_pFogObjects;
	int			                     m_nFog;

public:
	CSoftParticleShader();
	virtual ~CSoftParticleShader();


	virtual D3D12_BLEND_DESC         CreateBlendState();
	virtual D3D12_INPUT_LAYOUT_DESC  CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE    CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE    CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE    CreateGeometryShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_RASTERIZER_DESC    CreateRasterizerState();

	D3D12_SHADER_BYTECODE    CreateFogVertexShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE    CreateFogPixelShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE    CreateFogGeometryShader(ID3DBlob** ppd3dShaderBlob);

	void CreateNoiseTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets = 1);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, CHeightMapTerrain* pTerrain = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

};