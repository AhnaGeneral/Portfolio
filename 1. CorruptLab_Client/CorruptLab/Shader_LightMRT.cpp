#include "Shader_LightMRT.h"
#include "Object_Player.h"
#include "Mgr_Scene.h"
#include "Mgr_IndoorControl.h"
CLightTarget::CLightTarget()
{
	m_pTextures = NULL;

	m_pMaterials = NULL;
	m_pLights = NULL;

	m_pd3dcbLights = NULL;
	m_pcbMappedLights = NULL;

	m_pd3dcbMaterials = NULL;
	m_pcbMappedMaterials = NULL;

	m_pPlayer = NULL; 
}

CLightTarget::~CLightTarget()
{
	ReleaseShaderVariables();
	ReleaseObjects();
	if (m_pTextures)
	{
		m_pTextures->ReleaseShaderVariables();
	}
	m_pcbMappedLights = NULL;
	m_pcbMappedMaterials = NULL;
}

D3D12_DEPTH_STENCIL_DESC CLightTarget::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
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

void CLightTarget::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 6;
	pd3dDescriptorRanges[0].BaseShaderRegister = 1; //Texture[]
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[ROOT_PARAMETER_CDN_MRT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[ROOT_PARAMETER_CDN_MRT].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[ROOT_PARAMETER_CDN_MRT].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0]; //Texture
	pd3dRootParameters[ROOT_PARAMETER_CDN_MRT].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[ROOT_PARAMETER_LIGHT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[ROOT_PARAMETER_LIGHT].Descriptor.ShaderRegister = 3; //b3 : Lights
	pd3dRootParameters[ROOT_PARAMETER_LIGHT].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[ROOT_PARAMETER_LIGHT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[ROOT_PARAMETER_CAMERA2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[ROOT_PARAMETER_CAMERA2].Descriptor.ShaderRegister = 1; //b1 : Camera
	pd3dRootParameters[ROOT_PARAMETER_CAMERA2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[ROOT_PARAMETER_CAMERA2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.MipLODBias = 0;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc.ShaderRegister = 0;
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
}

void CLightTarget::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];
	ID3DBlob* pd3dVertexShaderBlob = NULL, * pd3dPixelShaderBlob = NULL;

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

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);
	//-----------------------------------------------------------------------

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;

}

void CLightTarget::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	m_pTextures = (CTexture*)pContext;
	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, FINAL_MRT_COUNT);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pTextures, ROOT_PARAMETER_CDN_MRT, true);
	BuildLightsAndMaterials();

}

void CLightTarget::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_LIGHT, d3dcbLightsGpuVirtualAddress); //Lights
}

void CLightTarget::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbMaterialBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbMaterials->Map(0, NULL, (void **)&m_pcbMappedMaterials);
}

void CLightTarget::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
	m_pd3dcbLights = NULL;
	if (m_pd3dcbMaterials)
	{
		m_pd3dcbMaterials->Unmap(0, NULL);
		m_pd3dcbMaterials->Release();
	}
	m_pd3dcbMaterials = NULL;

}

void CLightTarget::OutdoorRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	CShader::Render(pd3dCommandList, pCamera, nPipelineState);


	UpdateShaderVariables(pd3dCommandList);

	pCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA2);

	if (m_pTextures) m_pTextures->UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);
}

void CLightTarget::IndoorRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	CShader::Render(pd3dCommandList, pCamera, nPipelineState);
	
	UpdateShaderVariables(pd3dCommandList);

	pCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA2);

	if (m_pTextures) m_pTextures->UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);
}

void CLightTarget::ReleaseObjects()
{
	if (m_pMaterials)  delete m_pMaterials;
	if (m_pLights)  delete m_pLights;
}

void CLightTarget::ChangeLights()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.015f, 0.01f, 0.01f, 1.0f);

	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.025f, 0.025f, 0.025f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.0f, 0.1f, 0.15f, 1.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = Vector3::Normalize(XMFLOAT3(0.0f, -0.5f, -0.0f));
	m_pLights->m_pLights[0].m_bEnable = true;

	////m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	//m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	//m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	//m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	//m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.2f, 0.1f, 0.15f, 1.0f);
	//m_pLights->m_pLights[0].m_xmf3Direction = Vector3::Normalize(XMFLOAT3(0.0f, -1.0f, 0.0f));
	//m_pLights->m_pLights[0].m_bEnable = true;

	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_bEnable = false;
	m_pLights->m_pLights[1].m_fRange = 90.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.2f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.05f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(0.f, 80.0f, 0.f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.00003f);
	m_pLights->m_pLights[1].m_fFalloff = 4.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(70.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(40.0f));


	m_pLights->m_pLights[2].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[2].m_bEnable = false;
	m_pLights->m_pLights[2].m_fRange = 60.0f;
	m_pLights->m_pLights[2].m_xmf4Ambient  = XMFLOAT4(0.1f, 0.1f, 0.2f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.05f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[2].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.00003f);
	m_pLights->m_pLights[2].m_fFalloff = 4.0f;
	m_pLights->m_pLights[2].m_fPhi = (float)cos(XMConvertToRadians(50.0f));
	m_pLights->m_pLights[2].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	//TurnOnLabatoryLight();
	m_IndoorScene = true;

}

void CLightTarget::ChangeLights2()
{
	for (int i = 0; i < MAX_LIGHTS; i++)
		m_pLights->m_pLights[i].m_bEnable = false;

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.2f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.07f, 0.07f, 0.07f, 1.0f);


}

void CLightTarget::TurnOnLabatoryLight()
{
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.0f, 0.1f, 0.2f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.07f, 0.07f, 0.07f, 1.0f);
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);



	FILE* pInFile = NULL;
	::fopen_s(&pInFile, "ObjectsData/Laboratorys.bin", "rb");
	::rewind(pInFile);

	UINT nObjects = 0;
	(UINT)::fread_s(&nObjects, sizeof(UINT), sizeof(UINT), 1, pInFile);

	XMFLOAT3 xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f),
		xmf3Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f),
		xmf3Scale = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4 xmf4Rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4X4 xmmtxWorld;

	for (UINT i = 1; i < nObjects + 1; i++)
	{
		(UINT)::fread_s(&xmf3Position, sizeof(XMFLOAT3), sizeof(float), 3, pInFile);
		(UINT)::fread_s(&xmf3Rotation, sizeof(XMFLOAT3), sizeof(float), 3, pInFile); //Euler Angle
		(UINT)::fread_s(&xmf3Scale, sizeof(XMFLOAT3), sizeof(float), 3, pInFile);
		(UINT)::fread_s(&xmf4Rotation, sizeof(XMFLOAT4), sizeof(float), 4, pInFile); //Quaternion
		(UINT)::fread_s(&xmmtxWorld, sizeof(XMFLOAT4X4), sizeof(XMFLOAT4X4), 1, pInFile);

		m_pLights->m_pLights[2 + i].m_nType = SPOT_LIGHT;
		m_pLights->m_pLights[2 + i].m_bEnable = true;
		m_pLights->m_pLights[2 + i].m_fRange = 40.0f;
		m_pLights->m_pLights[2 + i].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.2f, 1.0f);
		m_pLights->m_pLights[2 + i].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0);
		m_pLights->m_pLights[2 + i].m_xmf4Specular = XMFLOAT4(0.05f, 0.1f, 0.26f, 1.0f);
		if (i >= 6)
		{
			m_pLights->m_pLights[2 + i].m_xmf3Position = XMFLOAT3(xmf3Position.x, xmf3Position.y - 8.0f, xmf3Position.z - 3.5f);
		}
		else
		{
			m_pLights->m_pLights[2 + i].m_xmf3Position = XMFLOAT3(xmf3Position.x, xmf3Position.y - 8.0f, xmf3Position.z + 3.5f);
		}
		m_pLights->m_pLights[2 + i].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_pLights->m_pLights[2 + i].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.00003f);
		m_pLights->m_pLights[2 + i].m_fFalloff = 3.0f;
		m_pLights->m_pLights[2 + i].m_fPhi = (float)cos(XMConvertToRadians(50.0f));
		m_pLights->m_pLights[2 + i].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	}

	if (pInFile) fclose(pInFile);

	m_pLights->m_pLights[13].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[13].m_bEnable = true;
	m_pLights->m_pLights[13].m_fRange = 60.0f;
	m_pLights->m_pLights[13].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.2f, 1.0f);
	m_pLights->m_pLights[13].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0);
	m_pLights->m_pLights[13].m_xmf4Specular = XMFLOAT4(0.05f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[13].m_xmf3Position = XMFLOAT3(103.0f, 6.5f, 3.0f);
	m_pLights->m_pLights[13].m_xmf3Direction = XMFLOAT3(0.99f, 0.0f, -0.12f);
	m_pLights->m_pLights[13].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.00003f);
	m_pLights->m_pLights[13].m_fFalloff = 4.0f;
	m_pLights->m_pLights[13].m_fPhi = (float)cos(XMConvertToRadians(50.0f));
	m_pLights->m_pLights[13].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights->m_pLights[14].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[14].m_bEnable = true;
	m_pLights->m_pLights[14].m_fRange = 40.0f;
	m_pLights->m_pLights[14].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.2f, 1.0f);
	m_pLights->m_pLights[14].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0);
	m_pLights->m_pLights[14].m_xmf4Specular = XMFLOAT4(0.1f, 0.3f, 0.2f, 0.0f);
	m_pLights->m_pLights[14].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[14].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.00003f);
	m_pLights->m_pLights[14].m_fFalloff = 3.0f;
	m_pLights->m_pLights[14].m_fPhi = (float)cos(XMConvertToRadians(100.0f));
	m_pLights->m_pLights[14].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
	m_pLights->m_pLights[14].m_xmf3Position = XMFLOAT3(125.0f, 25.0f, 1.0f);

	//m_pLights->m_pLights[15].m_nType = SPOT_LIGHT;
	//m_pLights->m_pLights[15].m_bEnable = true;
	//m_pLights->m_pLights[15].m_fRange = 50.0f;
	//m_pLights->m_pLights[15].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.2f, 1.0f);
	//m_pLights->m_pLights[15].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0);
	//m_pLights->m_pLights[15].m_xmf4Specular = XMFLOAT4(0.1f, 0.3f, 0.2f, 0.0f);
	//m_pLights->m_pLights[15].m_xmf3Direction = XMFLOAT3(0.0f, 0.2f, -1.0f);
	//m_pLights->m_pLights[15].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.00003f);
	//m_pLights->m_pLights[15].m_fFalloff = 4.0f;
	//m_pLights->m_pLights[15].m_fPhi = (float)cos(XMConvertToRadians(50.0f));
	//m_pLights->m_pLights[15].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	//m_pLights->m_pLights[15].m_xmf3Position = XMFLOAT3(0.28f, 10.0f, 23.0f);

	//m_pLights->m_pLights[16].m_nType = SPOT_LIGHT;
	//m_pLights->m_pLights[16].m_bEnable = true;
	//m_pLights->m_pLights[16].m_fRange = 50.0f;
	//m_pLights->m_pLights[16].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.2f, 1.0f);
	//m_pLights->m_pLights[16].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0);
	//m_pLights->m_pLights[16].m_xmf4Specular = XMFLOAT4(0.1f, 0.3f, 0.2f, 0.0f);
	//m_pLights->m_pLights[16].m_xmf3Direction = XMFLOAT3(0.0f, 0.2f, 1.0f);
	//m_pLights->m_pLights[16].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.00003f);
	//m_pLights->m_pLights[16].m_fFalloff = 4.0f;
	//m_pLights->m_pLights[16].m_fPhi = (float)cos(XMConvertToRadians(50.0f));
	//m_pLights->m_pLights[16].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	//m_pLights->m_pLights[16].m_xmf3Position = XMFLOAT3(0.28f, 10.0f, -23.0f);

	m_pLights->m_pLights[15].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[15].m_bEnable = true;
	m_pLights->m_pLights[15].m_fRange = 50.0f;
	m_pLights->m_pLights[15].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.2f, 1.0f);
	m_pLights->m_pLights[15].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0);
	m_pLights->m_pLights[15].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[15].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[15].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.00003f);
	m_pLights->m_pLights[15].m_fFalloff = 4.0f;
	m_pLights->m_pLights[15].m_fPhi = (float)cos(XMConvertToRadians(60.0f));
	m_pLights->m_pLights[15].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
	m_pLights->m_pLights[15].m_xmf3Position = XMFLOAT3(0.0f, 30.0f, 0.0f);



}

void CLightTarget::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	//m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.55f, 0.5f, 0.4f, 1.0f);
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.1f, 0.05f, 1.0f);

	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.2f, 0.1f, 0.15f, 1.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = Vector3::Normalize(XMFLOAT3(1.0f, -0.5f, -1.0f));
	m_pLights->m_pLights[0].m_bEnable = true;

	m_pLights->m_pLights[1].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.6f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.0f, 0.f, 1.f, 1.0f);
	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_fRange = 30;
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.001f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(368, 53.0f, 107);


	m_pLights->m_pLights[2].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.2f, 1.0f, 0.2f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_fRange = 30;
	m_pLights->m_pLights[2].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.001f);
	m_pLights->m_pLights[2].m_xmf3Position = XMFLOAT3(430.8f, 40.8f, 205.0f);

	m_pLights->m_pLights[3].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.2f, 1.0f, 0.2f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_fRange = 30;
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.001f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(220.0f, 100.f, 277.0f);


	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));

	m_pMaterials->m_pReflections[0] = { XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[1] = { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[2] = { XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[3] = { XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[4] = { XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 25.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[5] = { XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[6] = { XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 35.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[7] = { XMFLOAT4(1.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };


}


D3D12_SHADER_BYTECODE CLightTarget::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_LightTarget.hlsl", "VSLightTarget", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CLightTarget::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"HLSL_LightTarget.hlsl", "PSLightTargeet", "ps_5_1", ppd3dShaderBlob));
}


void CLightTarget::AnimateObjects(float fTimeElapsed)
{
	if (m_IndoorScene)
	{
		if (m_pLights && CMgr_IndoorControl::GetInstance()->GetExecuteHandLightControl())
		{
			m_pLights->m_pLights[2].m_bEnable = true;
			m_pLights->m_pLights[2].m_xmf3Position = XMFLOAT3(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y + 3.0f, m_pPlayer->GetPosition().z);
			m_pLights->m_pLights[2].m_xmf3Direction = m_pPlayer->GetLookVector();
		}
		else
		{
			m_pLights->m_pLights[2].m_bEnable = false;
		}

		if (m_bTimeControl)
		{
			m_fTime += fTimeElapsed;

			if (m_fTime > 6.0f)
			{
				m_pLights->m_pLights[1].m_bEnable = false;
				m_bTimeControl = false;
			}
		}
	}
}
