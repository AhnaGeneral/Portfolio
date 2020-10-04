#pragma once
#include "Shader.h"

class Shader_Noise : public CShader
{
public:
    Shader_Noise();
    virtual ~Shader_Noise();

public:

    virtual D3D12_SHADER_BYTECODE    CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_SHADER_BYTECODE    CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_RASTERIZER_DESC    CreateRasterizerState();

    virtual D3D12_INPUT_LAYOUT_DESC  CreateInputLayout();

    virtual D3D12_BLEND_DESC         CreateBlendState();

    virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets = 1);
    virtual void ReleaseObjects();

};

class Shader_Basic : public Shader_Noise
{
public:
    Shader_Basic();
    virtual ~Shader_Basic();

public:
    virtual D3D12_SHADER_BYTECODE    CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_SHADER_BYTECODE    CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_DEPTH_STENCIL_DESC  CreateDepthStencilState();

};

class Shader_TextureBasic : public Shader_Basic
{
public:
    Shader_TextureBasic();
    virtual ~Shader_TextureBasic();

public:
    virtual D3D12_SHADER_BYTECODE    CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_SHADER_BYTECODE    CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

};
class Shader_WaterNoise : public Shader_Noise
{
public:
    Shader_WaterNoise();
    virtual ~Shader_WaterNoise();

    virtual D3D12_SHADER_BYTECODE    CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_SHADER_BYTECODE    CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
};
