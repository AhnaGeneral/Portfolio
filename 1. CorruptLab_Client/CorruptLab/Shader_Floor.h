#pragma once
#include "Shader_BaseUI.h"
class CFloorShader :
    public CShader_BaseUI
{

    virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

    virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
    virtual D3D12_SHADER_BYTECODE CreateShadowPixelShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(ID3DBlob** ppd3dShaderBlob);

    virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets = 1);
};

