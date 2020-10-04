#include "stdafx.h"
#include "Object_Nosie.h"
#include "Shader_Noise.h"
#include "Mesh.h"
#include "Mgr_Radiation.h"
#include "Mgr_Item.h"
#include "Mgr_Collision.h"

CObjectNosie::CObjectNosie()
{
	m_fFrameTime = 0.0f;
	m_fPosition = { 0.0f, 0.0f, 0.0f };
	
}

CObjectNosie::CObjectNosie(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 position, CShader* pShader)
{
	m_pd3dPositionBuffer =
		::CreateBufferResource(pd3dDevice, pd3dCommandList, &position, sizeof(XMFLOAT3), 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) ;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CObjectNosie::~CObjectNosie()
{
	ReleaseShaderVariables();
}

void CObjectNosie::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// GameObject[게임 오브젝트 콘스탄트버퍼] -------------------------------------------------------------
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);

	// noisebuffer [노이즈의 콘스탄트버퍼] ----------------------------------------------------------------
	ncbElementBytes = ((sizeof(CB_NOISEBUFFERTYPE) + 255) & ~255);
	m_pd3dcbNoiseBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbNoiseBuffer->Map(0, NULL, (void**)&m_pcbMappedNoiseBuffers);

	// distort [외각 콘스탄트버퍼] ------------------------------------------------------------------------
	ncbElementBytes = ((sizeof(CB_DISTORTIONBUFFERTYPE) + 255) & ~255);
	m_pd3dcbDistortionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbDistortionBuffer->Map(0, NULL, (void**)&m_pcbMappdeDistortBuffers);

	ncbElementBytes = ((sizeof(int) + 255) & ~255);
	m_pd3dRadiationLevel = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	//m_iRadiationLevel = new int;
	m_pd3dRadiationLevel->Map(0, NULL, (void**)&m_iRadiationLevel);
	*m_iRadiationLevel = CRadationMgr::GetInstance()->GetStrength(m_fPosition);

}

void CObjectNosie::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	
}

void CObjectNosie::ReleaseShaderVariables()
{
	// GameObject[게임 오브젝트 콘스탄트버퍼] -------------------------------------------------------------
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	//if (m_pcbMappedGameObjects) delete m_pcbMappedGameObjects;

	// noisebuffer [노이즈의 콘스탄트버퍼] ----------------------------------------------------------------
	if (m_pd3dcbNoiseBuffer)
	{
		m_pd3dcbNoiseBuffer->Unmap(0, NULL);
		m_pd3dcbNoiseBuffer->Release();
	}
	//if (m_pcbMappedNoiseBuffers) delete m_pcbMappedNoiseBuffers;

	// distort [외각 콘스탄트버퍼] ------------------------------------------------------------------------
	if (m_pd3dcbDistortionBuffer)
	{
		m_pd3dcbDistortionBuffer->Unmap(0, NULL);
		m_pd3dcbDistortionBuffer->Release();
	}

	if (m_pd3dPositionBuffer)
	{
	//	m_pd3dPositionBuffer->Unmap(0, NULL);
		m_pd3dPositionUploadBuffer->Release();
		m_pd3dPositionBuffer->Release();
	}
	//if (m_pcbMappdeDistortBuffers) delete m_pcbMappdeDistortBuffers;

	if (m_pd3dRadiationLevel)
	{
		m_pd3dRadiationLevel->Unmap(0, NULL);
		m_pd3dRadiationLevel->Release();
	}
}


void CObjectNosie::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	// 프레임 시간 카운터를 증가시킵니다.
	m_fFrameTime += 0.01f;
	if (m_fFrameTime > 1000.0f)
	{
		m_fFrameTime = 0.0f;
	}

	if (CItemMgr::GetInstance()->GetItemNums().w == 1.0f && ControlFog!= PROGRESS_FILED2)
	{
		m_FogTime += 0.1f;

		if (m_FogTime > 20.0f)
		{
		    CCollisionMgr::GetInstance()->m_iSceneProgress = PROGRESS_FILED2;
			ControlFog = PROGRESS_FILED2;
			m_FogTime = 3.0f;
		}
	}

	if (CItemMgr::GetInstance()->GetItemNums().w == 2.0f && ControlFog != PROGRESS_FILED3)
	{
		m_FogTime += 0.1f;

		if (m_FogTime > 20.0f)
		{
			CCollisionMgr::GetInstance()->m_iSceneProgress = PROGRESS_FILED3;
			ControlFog = PROGRESS_FILED3;
			m_FogTime = 3.0f;
		}
	}

	// GameObject[게임 오브젝트 콘스탄트버퍼] -------------------------------------------------------------
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

	UINT getobjectID = 0;
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 4, &getobjectID, 16);

	// noisebuffer [노이즈의 콘스탄트버퍼] ----------------------------------------------------------------
	::memcpy(&m_pcbMappedNoiseBuffers->frameTime, &m_fFrameTime, sizeof(float));
	::memcpy(&m_pcbMappedNoiseBuffers->scrollSpeeds, &m_cbvNoisebuffer.scrollSpeeds, sizeof(XMFLOAT3));
	::memcpy(&m_pcbMappedNoiseBuffers->scales, &m_cbvNoisebuffer.scales, sizeof(XMFLOAT3));
	::memcpy(&m_pcbMappedNoiseBuffers->padding, &m_cbvNoisebuffer.padding, sizeof(float));
	::memcpy(&m_pcbMappedNoiseBuffers->Angle, &m_cbvNoisebuffer.Angle, sizeof(XMFLOAT3));
	::memcpy(&m_pcbMappedNoiseBuffers->FogTime, &m_FogTime, sizeof(float));


	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbNoiseBuffer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_NOISEBUFFER, d3dGpuVirtualAddress);

	//std::cout << "frameTime "    << m_pcbMappedNoiseBuffers->frameTime  << std::endl;
	//std::cout << "scrollSpeeds " << m_pcbMappedNoiseBuffers->scrollSpeeds.x << ", " 
	//	                         << m_pcbMappedNoiseBuffers->scrollSpeeds.y <<", "
	//	                         << m_pcbMappedNoiseBuffers->scrollSpeeds.z << std::endl;


	// distort [외각 콘스탄트버퍼] ------------------------------------------------------------------------
	::memcpy(&m_pcbMappdeDistortBuffers->distortion1, &m_cbvdistortion.distortion1, sizeof(XMFLOAT2));
	::memcpy(&m_pcbMappdeDistortBuffers->distortion2, &m_cbvdistortion.distortion2, sizeof(XMFLOAT2));
	::memcpy(&m_pcbMappdeDistortBuffers->distortion3, &m_cbvdistortion.distortion3, sizeof(XMFLOAT2));
	::memcpy(&m_pcbMappdeDistortBuffers->distortionScale, &m_cbvdistortion.distortionScale, sizeof(float));
	::memcpy(&m_pcbMappdeDistortBuffers->distortionBias, &m_cbvdistortion.distortionBias, sizeof(float));

	d3dGpuVirtualAddress = m_pd3dcbDistortionBuffer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_DISTORTIONBUFFER, d3dGpuVirtualAddress);

	//Radiation-----------------------------------------------------------------------------

	d3dGpuVirtualAddress = m_pd3dRadiationLevel->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_RADIATIONLEVEL, d3dGpuVirtualAddress);

	//std::cout << "distortion2 " << m_pcbMappdeDistortBuffers->distortion2.x << ", " << m_pcbMappdeDistortBuffers->distortion2.y << std::endl;
	//std::cout << "distortion3 " << m_pcbMappdeDistortBuffers->distortion3.x << ", " << m_pcbMappdeDistortBuffers->distortion3.y << std::endl;
}

void CObjectNosie::GenerateShaderDistortionBuffer()
{
	m_cbvdistortion.distortion1 = XMFLOAT2(0.1f, 0.2f);
	m_cbvdistortion.distortion2 = XMFLOAT2(0.1f, 0.3f);
	m_cbvdistortion.distortion3 = XMFLOAT2(0.1f, 0.1f);
	m_cbvdistortion.distortionScale = 0.8f;
	m_cbvdistortion.distortionBias = 0.5f;

	m_cbvNoisebuffer.scrollSpeeds = XMFLOAT3(1.f, 1.1f, 1.3f);
	m_cbvNoisebuffer.padding = 0.0;
	m_cbvNoisebuffer.scales = XMFLOAT3(1.0f, 2.0f, 3.0f);

	// 처음부터 초기화 할시.
	//   m_pcbMappdeDistortBuffers->distortion1 = XMFLOAT2(0.1f, 0.2f);
	//   m_pcbMappdeDistortBuffers->distortion2 = XMFLOAT2(0.1f, 0.3f);
	//   m_pcbMappdeDistortBuffers->distortion3 = XMFLOAT2(0.1f, 0.1f);
	//   m_pcbMappdeDistortBuffers->distortionScale = 0.8f; 
	//   m_pcbMappdeDistortBuffers->distortionBias = 0.5f; 
}

void CObjectNosie::UpdateShaderNoiseBuffer(float Time, XMFLOAT3 scrollspeed, XMFLOAT3 scales, float padding)
{
	m_cbvNoisebuffer.frameTime = Time;
	m_cbvNoisebuffer.scrollSpeeds = scrollspeed;
	m_cbvNoisebuffer.scales = scales;
	m_cbvNoisebuffer.padding = padding;
}


void CObjectNosie::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	// 2020 03 23 텍스처 여러개 불렀을 때 해결 방법
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[1] = { m_d3dPositionBufferView };
	pd3dCommandList->IASetVertexBuffers(0, _countof(pVertexBufferViews), pVertexBufferViews);
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	pd3dCommandList->DrawInstanced(1, 1, 0, 0);
}

CObjectFog::CObjectFog(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 position, CShader* pShader)
	//: CObjectNosie(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	m_fPosition = position ;
	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &position, sizeof(XMFLOAT3),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);

}

CObjectFog::~CObjectFog()
{
	//ReleaseShaderVariables();
}

void CObjectFog::GenerateShaderDistortionBuffer()
{
	m_cbvdistortion.distortion1 = XMFLOAT2(0.1f, 0.2f);
	m_cbvdistortion.distortion2 = XMFLOAT2(0.1f, 0.3f);
	m_cbvdistortion.distortion3 = XMFLOAT2(0.1f, 0.1f);
	m_cbvdistortion.distortionScale = 0.8f;
	m_cbvdistortion.distortionBias = 0.5f;

	m_cbvNoisebuffer.scrollSpeeds = XMFLOAT3(0.02f, 0.01f, 2.3f);
	m_cbvNoisebuffer.padding = 0.0;
	m_cbvNoisebuffer.scales = XMFLOAT3(1.0f, 2.0f, 3.0f);
}


CObjectWater::CObjectWater(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
	//: CObjectNosie(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	CTriangleRect* pNoise = new CTriangleRect(pd3dDevice, pd3dCommandList, 2000.0f, 2000.0f, 0.0f, 1.0f);
	SetMesh(pNoise);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	NoiseSetTexture(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

CObjectWater::~CObjectWater()
{
	//ReleaseShaderVariables();
}

void CObjectWater::NoiseSetTexture(ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CTexture* pNoiseTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pNoiseTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Geometry/Water/Water_NM.dds", 0);

	Shader_WaterNoise* pNosieShader = new Shader_WaterNoise();
	pNosieShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	pNosieShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pNosieShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1); // 힙이 달라서... OnPrepareRender
	pNosieShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pNoiseTexture, ROOT_PARMAMETER_WATER_NORMAL_TEX, false);
	SetShader(pNosieShader);
	m_ppMaterials[0]->SetTexture(pNoiseTexture);
}

void CObjectWater::GenerateShaderDistortionBuffer()
{
	m_cbvdistortion.distortion1 = XMFLOAT2(0.1f, 0.2f);
	m_cbvdistortion.distortion2 = XMFLOAT2(0.1f, 0.3f);
	m_cbvdistortion.distortion3 = XMFLOAT2(0.1f, 0.1f);
	m_cbvdistortion.distortionScale = 0.8f;
	m_cbvdistortion.distortionBias = 0.5f;

	m_cbvNoisebuffer.scrollSpeeds = XMFLOAT3(0.8f, 2.1f, 0.05f);
	m_cbvNoisebuffer.padding = 0.0;
	m_cbvNoisebuffer.scales = XMFLOAT3(2.0f, 20.0f, 30.0f);
}

void CObjectWater::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera, 0);
	m_ppMaterials[0]->UpdateShaderVariable(pd3dCommandList);

	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, 0);
}

