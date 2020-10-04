#include "stdafx.h"
#include "Shader_Lobby.h"

Shader_Lobby::~Shader_Lobby()
{
	ReleaseUploadBuffers();
	ReleaseObjects();
}

D3D12_SHADER_BYTECODE Shader_Lobby::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Lobby.hlsl", "VSLobbyScene", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE Shader_Lobby::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Lobby.hlsl", "PSLobbyScene", "ps_5_1", ppd3dShaderBlob));
}

D3D12_INPUT_LAYOUT_DESC Shader_Lobby::CreateInputLayout()
{
	return D3D12_INPUT_LAYOUT_DESC();
}

D3D12_DEPTH_STENCIL_DESC Shader_Lobby::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = false;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

void Shader_Lobby::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	ID3DBlob   * pd3dVertexShaderBlob = NULL, 
		       * pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = nRenderTargets;

	for (UINT i = 0; i < nRenderTargets; i++)
		d3dPipelineStateDesc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;

	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void Shader_Lobby::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	CShader::Render(pd3dCommandList, pCamera);
	m_pLobbyMaterial->m_ppTextures[0]->UpdateShaderVariables(pd3dCommandList);
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);

}

void Shader_Lobby::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Lobby/MainScene.dds", 0);

	m_pLobbyMaterial = new CMaterial(1);
	m_pLobbyMaterial->SetTexture(pUITexture);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 1); 
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pUITexture, ROOT_PARAMETER_LOBBYTEX, false);
	
}

void Shader_Lobby::ReleaseObjects()
{
	if (m_pLobbyMaterial)
	{
		m_pLobbyMaterial->m_ppTextures[0]->ReleaseShaderVariables();
		m_pLobbyMaterial->ReleaseUploadBuffers();
		m_pLobbyMaterial->Release();
		m_pLobbyMaterial = NULL;
	}
}

void Shader_Lobby::ReleaseUploadBuffers()
{
}
