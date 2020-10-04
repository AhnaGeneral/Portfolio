#pragma once
#include "Shader_Minimap.h"
class CBillboardUIShader :public CMinimapShader
{
public: 

	CBillboardUIShader() {}
	virtual ~CBillboardUIShader() {}

	virtual D3D12_SHADER_BYTECODE    CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE    CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_BLEND_DESC	     CreateBlendState();
	virtual D3D12_RASTERIZER_DESC    CreateRasterizerState();

};

