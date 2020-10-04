#include "Shader_Effect.h"

CShader_Effect::CShader_Effect()
{
}

CShader_Effect::~CShader_Effect()
{
}

D3D12_BLEND_DESC CShader_Effect::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	return(d3dBlendDesc);
}

D3D12_INPUT_LAYOUT_DESC CShader_Effect::CreateInputLayout()
{
	UINT nInputEffectElementDescs = 1; 
	D3D12_INPUT_ELEMENT_DESC* pd3dInputEffectElementDesc = new D3D12_INPUT_ELEMENT_DESC[nInputEffectElementDescs];

	pd3dInputEffectElementDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputEffectElementDesc;
	d3dInputLayoutDesc.NumElements = nInputEffectElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CShader_Effect::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Effect.hlsl", "EffectVertexShader", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader_Effect::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Effect.hlsl", "EffectPixelShader", "ps_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader_Effect::CreateGeometryShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Effect.hlsl", "EffectGS", "gs_5_1", ppd3dShaderBlob));
}

D3D12_DEPTH_STENCIL_DESC CShader_Effect::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
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

D3D12_RASTERIZER_DESC CShader_Effect::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_SHADER_BYTECODE CShader_Effect::CreateSPRVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Effect.hlsl", "EffectVertexShader", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader_Effect::CreateSPRPixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Effect.hlsl", "EffectSPTPixelShader", "ps_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader_Effect::CreateSPRXZGeometryShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Effect.hlsl", "EffectSPTGS", "gs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader_Effect::CreateSPRYZGeometryShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Effect.hlsl", "EffectSPTYZGS", "gs_5_1", ppd3dShaderBlob));
}

void CShader_Effect::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_SPTEFFECTTYPE) + 255) & ~255);
	m_pd3dcbEffectBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbEffectBuffer->Map(0, NULL, (void**)&m_pcbMappedEffectBuffers);
}

void CShader_Effect::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_spt.LifeTime += 0.02f;

	m_spt.LoopTime += m_SPTTimeSpeed;

	if (m_spt.LoopTime > 5.0f)
	{
		m_spt.LoopTime = 0.0f;

		if (m_spt.row > 6)
		{
			m_spt.row = 0;
			m_spt.col += 1;
		}

		if (m_spt.col > 6)
		{
			m_spt.col = 0;
		}

		m_spt.row += 1;
	}

	::memcpy(&m_pcbMappedEffectBuffers->LifeTime, &m_spt.LifeTime, sizeof(float));
	::memcpy(&m_pcbMappedEffectBuffers->LoopTime, &m_spt.LoopTime, sizeof(float));
	::memcpy(&m_pcbMappedEffectBuffers->row, &m_spt.row, sizeof(int));
	::memcpy(&m_pcbMappedEffectBuffers->col, &m_spt.col, sizeof(int));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbEffectBuffer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_EFFECTCBV, d3dGpuVirtualAddress);
}

void CShader_Effect::ReleaseShaderVariables()
{
}

void CShader_Effect::CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pEffectTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0); 
	m_pEffectTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Effect/T_Light01.dds", 0);
	m_pEffectTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Effect/SPT_Wave02.dds", 1);
	m_pEffectTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Effect/SPT_Tiping.dds", 2);

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pEffectTexture, ROOT_PARAMETER_EFFECT, false);
}

void CShader_Effect::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets)
{
	m_nPipelineStates = 3;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	//FogPipeline

	ID3DBlob * pd3dVertexShaderBlob = NULL, * pd3dPixelShaderBlob = NULL, * pd3dGeometryShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.GS = CreateGeometryShader(&pd3dGeometryShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	d3dPipelineStateDesc.NumRenderTargets = nRenderTargets;

	for (UINT i = 0; i < nRenderTargets; i++)
		d3dPipelineStateDesc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;

	d3dPipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;

	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, 
		__uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

	//=============================================================
	d3dPipelineStateDesc.VS = CreateSPRVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.GS = CreateSPRXZGeometryShader(&pd3dGeometryShaderBlob);
	d3dPipelineStateDesc.PS = CreateSPRPixelShader(&pd3dPixelShaderBlob);

	hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[1]);

	//=============================================================
	d3dPipelineStateDesc.GS = CreateSPRYZGeometryShader(&pd3dGeometryShaderBlob);

	hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[2]);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
	if (pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;

}

void CShader_Effect::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 _pos, bool GreenZoon, float _SPTTimeSpeed)

{
	CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 3); 
	
	if (GreenZoon)
	{
		m_pLight01obj = new CObject_Effect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
			XMFLOAT3(_pos.x, _pos.y + 2, _pos.z), this);
		m_pLight01obj->UpdatePosition(XMFLOAT3(_pos.x, _pos.y + 2, _pos.z));

		m_pSPT_Wave02obj = new CObject_Effect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
			XMFLOAT3(_pos.x, _pos.y, _pos.z), this);
		m_pSPT_Wave02obj->UpdatePosition(XMFLOAT3(_pos.x, _pos.y, _pos.z));

	}
	else
	{
		m_pSPT_Tiping = new CObject_Effect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
			XMFLOAT3(_pos.x, _pos.y, _pos.z), this);
		m_pSPT_Tiping->UpdatePosition(XMFLOAT3(_pos.x, _pos.y, _pos.z));
	}

	m_spt.col = 0;
	m_spt.row = 0;

	m_SPTTimeSpeed = _SPTTimeSpeed;

	CreateTexture(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature); 
	CreateShaderVariables(pd3dDevice, pd3dCommandList); 
	ReleaseUploadBuffers(); 
}

void CShader_Effect::UpdatePosition(XMFLOAT3 _pos)
{
	if (m_pSPT_Tiping)m_pSPT_Tiping->UpdatePosition(_pos);
}

void CShader_Effect::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool GreenZoon)
{
	if (GreenZoon)
	{
		OnPrepareRender(pd3dCommandList, 0);
		m_pEffectTexture->UpdateShaderVariable(pd3dCommandList, 0);
		m_pLight01obj->Render(pd3dCommandList, pCamera);
		OnPrepareRender(pd3dCommandList, 1);
		m_pEffectTexture->UpdateShaderVariable(pd3dCommandList, 1);
		m_pSPT_Wave02obj->Render(pd3dCommandList, pCamera);
	}
	else
	{
		OnPrepareRender(pd3dCommandList, 2);
		m_pEffectTexture->UpdateShaderVariable(pd3dCommandList, 2);
		m_pSPT_Tiping->Render(pd3dCommandList, pCamera);
	}

}

void CShader_Effect::ReleaseObjects()
{
}

void CShader_Effect::ReleaseUploadBuffers()
{
	if(m_pLight01obj)
		m_pLight01obj->ReleaseUploadBuffers();
}
