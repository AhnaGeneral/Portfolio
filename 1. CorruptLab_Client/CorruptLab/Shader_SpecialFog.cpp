#include "stdafx.h"
#include "Shader_SpecialFog.h"
#include "Object_Nosie.h"
#include "Mgr_Collision.h"
#include "Mgr_Item.h"
D3D12_INPUT_LAYOUT_DESC CShader_SpecialFog::CreateInputLayout()
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

void CShader_SpecialFog::CreateNoiseTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pFireNoiseTextures = new CTexture(3, RESOURCE_TEXTURE2D, 0);
	m_pFireNoiseTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Noise/fire01.dds", 0);
	m_pFireNoiseTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Noise/alpha01.dds", 1);
	m_pFireNoiseTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Noise/noise01.dds", 2);

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pFireNoiseTextures, ROOT_PARAMETER_NOISE_TEX, true);


	m_pFogNoiseTextures = new CTexture(3, RESOURCE_TEXTURE2D, 0);
	m_pFogNoiseTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Fog/Blur.dds", 0);
	m_pFogNoiseTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Fog/Fog.dds", 1);
	m_pFogNoiseTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Fog/Fog2.dds", 2);

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pFogNoiseTextures, ROOT_PARAMETER_FOG_TEX, true);

}

D3D12_RASTERIZER_DESC CShader_SpecialFog::CreateRasterizerState()
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

D3D12_SHADER_BYTECODE CShader_SpecialFog::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Noise.hlsl", "NoiseVertexShader", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader_SpecialFog::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Noise.hlsl", "NoisePixelShader", "ps_5_1", ppd3dShaderBlob));
}


D3D12_SHADER_BYTECODE CShader_SpecialFog::CreateGeometryShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Noise.hlsl", "GS", "gs_5_1", ppd3dShaderBlob));
}

D3D12_DEPTH_STENCIL_DESC CShader_SpecialFog::CreateDepthStencilState()
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

CShader_SpecialFog::CShader_SpecialFog()
{
	m_pSceneDepthTextures = NULL;
	m_pFireNoiseTextures = NULL;
	m_pFogNoiseTextures = NULL;

	m_pFireObjects = NULL;
	m_nFire = 0;

	m_pFogObjects = NULL;
	m_pOneStageFog = NULL;
	m_pTwoStageFog = NULL;
	m_nFog = 0;
}

CShader_SpecialFog::~CShader_SpecialFog()
{
	ReleaseObjects();
}

D3D12_BLEND_DESC CShader_SpecialFog::CreateBlendState()
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

D3D12_SHADER_BYTECODE CShader_SpecialFog::CreateFogVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Fog.hlsl", "FogVertexShader", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader_SpecialFog::CreateFogPixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Fog.hlsl", "SpecialFogPixelShader", "ps_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader_SpecialFog::CreateFogGeometryShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_Fog.hlsl", "SpecialGS", "gs_5_1", ppd3dShaderBlob));
}


void CShader_SpecialFog::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets)
{

	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];
	//FogPipeline

	ID3DBlob* pd3dVertexShaderBlob = NULL, * pd3dPixelShaderBlob = NULL, * pd3dGeometryShaderBlob = NULL;

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

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

	//-------------------------------------------------------------------------------

	d3dPipelineStateDesc.VS = CreateFogVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreateFogPixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.GS = CreateFogGeometryShader(&pd3dPixelShaderBlob);

	hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[1]);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
	if (pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;



}

void CShader_SpecialFog::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, CHeightMapTerrain* pTerrain)
{
	CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 8);

	m_pSceneDepthTextures = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	m_pSceneDepthTextures->SetTexture(0, pContext);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pSceneDepthTextures, ROOT_PARAMETER_SCENEDEPTHTEX, 0);

	// ===========================================================================================
	m_nOneFog = 14;
	m_pOneStageFog = new CObjectNosie * [m_nOneFog];
	int one = 0;
	CObjectNosie* poneNoise;
	
	for (int i = 0; i < m_nOneFog; i ++)
	{
		int Xpos = rand() % 40 + 315; //280 ~ 320
		int Ypos = rand() % 400 + 56; //56 ~ 456
		float fHeight = 0.0f; 
		if (pTerrain)
		{
			fHeight = pTerrain->GetHeight(float(Xpos), float(Ypos)) + 40;
		}
			poneNoise = new CObjectFog(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
				XMFLOAT3(float(Xpos), fHeight, float(Ypos)), this);  //object
			poneNoise->GenerateShaderDistortionBuffer();
			poneNoise->SetAngle(XMFLOAT3(1, 0, 0));
			m_pOneStageFog[one++] = poneNoise;
	}

	m_nTwoFog = 17;
	m_pTwoStageFog = new CObjectNosie * [m_nTwoFog];
	int two = 0;
	CObjectNosie* ptwoNoise;
	for (int i = 0; i < 7; i++)
	{
		int Xpos = rand() % 250 + 20; //20 ~ 270 
		int Ypos = rand() % 40 + 210;
		float fHeight = 0.0f;
		if (pTerrain)
		{
			fHeight = pTerrain->GetHeight(float(Xpos), float(Ypos)) + 40;
		}
			ptwoNoise = new CObjectFog(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
				XMFLOAT3(float(Xpos), fHeight, float (Ypos)), this);  //object
			ptwoNoise->GenerateShaderDistortionBuffer();
			ptwoNoise->SetAngle(XMFLOAT3(0, 0, 1));
			m_pTwoStageFog[two++] = ptwoNoise;
	}
	for (int i = 7; i < m_nTwoFog; i++)
	{
		int Xpos = rand() % 40 + 320; // 320 ~ 360
		int Ypos = rand() % 280 + 200; // 200 ~ 480 
		float fHeight = 0.0f; 
		if (pTerrain)
		{
			fHeight = pTerrain->GetHeight(float(Xpos), float(Ypos)) + 40;
		}
		poneNoise = new CObjectFog(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
			XMFLOAT3(float(Xpos), fHeight, float(Ypos)), this);  //object
		poneNoise->GenerateShaderDistortionBuffer();
		poneNoise->SetAngle(XMFLOAT3(1, 0, 0));
		m_pTwoStageFog[i] = poneNoise;
	}

	CreateNoiseTexture(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	ReleaseUploadBuffers();

}

void CShader_SpecialFog::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
    OnPrepareRender(pd3dCommandList);
	m_pSceneDepthTextures->UpdateShaderVariables(pd3dCommandList);

	//m_pFireNoiseTextures->UpdateShaderVariable(pd3dCommandList, 0);
	//for (int i = 0; i < m_nFire; i++)
	//	m_pFireObjects[i]->Render(pd3dCommandList, pCamera);

	OnPrepareRender(pd3dCommandList, 1);

	m_pFogNoiseTextures->UpdateShaderVariable(pd3dCommandList, 0);

	int Progress = CCollisionMgr::GetInstance()->m_iSceneProgress;
	float StageFog = CItemMgr::GetInstance()->GetItemNums().w;
	
	switch (Progress)
	{
	case PROGRESS_FILED1:
		for (int i = 0; i < m_nOneFog; i++)
			m_pOneStageFog[i]->Render(pd3dCommandList, pCamera);
		break;
	case PROGRESS_FILED2:
		for (int i = 0; i < m_nTwoFog; i++)
			m_pTwoStageFog[i]->Render(pd3dCommandList, pCamera);

	case PROGRESS_FILED3:
	/*	for (int i = 0; i < m_nTwoFog; i++)
			m_pTwoStageFog[i]->Render(pd3dCommandList, pCamera);*/
		break;
	default:
		break;
	}
}

void CShader_SpecialFog::ReleaseObjects()
{
	for (int i = 0; i < m_nFire; i++)
	{
		m_pFireObjects[i]->Release();
	}
	delete[] m_pFireObjects;


	for (int i = 0; i < m_nFog; i++)
	{
		m_pFogObjects[i]->Release();
	}
	delete[] m_pFogObjects;

	for (int i = 0; i < m_nOneFog; i++)
	{
		m_pOneStageFog[i]->Release();
	}
	delete[] m_pOneStageFog;

	for (int i = 0; i < m_nTwoFog; i++)
	{
		m_pTwoStageFog[i]->Release();
	}
	delete[] m_pTwoStageFog;

	//if (m_pSceneDepthTextures)m_pSceneDepthTextures->ReleaseUploadBuffers();
	//if (m_pFireNoiseTextures)m_pFireNoiseTextures->ReleaseUploadBuffers();
	//if (m_pFogNoiseTextures)m_pFogNoiseTextures->ReleaseUploadBuffers();

	//if (m_pSceneDepthTextures)m_pSceneDepthTextures->Release();
	//if (m_pFireNoiseTextures)m_pFireNoiseTextures->Release();
	//if (m_pFogNoiseTextures)m_pFogNoiseTextures->Release();
}

void CShader_SpecialFog::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nFire; i++)
	{
		m_pFireObjects[i]->ReleaseUploadBuffers();
	}

	for (int i = 0; i < m_nOneFog; i++)
	{
		m_pOneStageFog[i]->ReleaseUploadBuffers();
	}
	for (int i = 0; i < m_nTwoFog; i++)
	{
		m_pTwoStageFog[i]->ReleaseUploadBuffers();
	}
}