#pragma once
#include "Shader.h"

struct GS_COLLISION_BOX_INFO
{
	XMFLOAT3  m_xmf3Center;
	XMFLOAT3  m_xmf3Extent;
	XMFLOAT4  m_xmf4Orientation;
};

class Shader_CollisionBox : public CShader
{
public:
	Shader_CollisionBox();
	virtual ~Shader_CollisionBox();

	virtual D3D12_SHADER_BYTECODE    CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE    CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE    CreateGeometryShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_INPUT_LAYOUT_DESC  CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC	 CreateRasterizerState();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets = 1);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};

