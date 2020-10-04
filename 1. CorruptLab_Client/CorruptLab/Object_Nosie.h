#pragma once
#include "Object.h"

//Nosie ===========================================================================================================
struct CB_NOISEBUFFERTYPE
{
	float       frameTime = 0.0f;
	XMFLOAT3    scrollSpeeds = {0.0f, 0.0f, 0.0f};
	XMFLOAT3    scales = { 0.0f, 0.0f, 0.0f };
	float       padding = 0.0f;
	XMFLOAT3	Angle = { 0,0,0 };
	float       FogTime = 4.0f;
};

struct CB_DISTORTIONBUFFERTYPE
{
	XMFLOAT2    distortion1 = {0.0f, 0.0f};
	XMFLOAT2    distortion2 = { 0.0f, 0.0f };
	XMFLOAT2    distortion3 = { 0.0f, 0.0f };
	float       distortionScale = 0.0f;
	float       distortionBias = 0.0f;
};

class CObjectNosie : public CGameObject
{
protected:
	CB_DISTORTIONBUFFERTYPE     m_cbvdistortion;
	CB_NOISEBUFFERTYPE          m_cbvNoisebuffer;
	float					    m_fFrameTime = 0.0f;
	float                       m_FogTime = 1.0f;
	int                         ControlFog = 0;
	XMFLOAT3				    m_fPosition = { 0.0f, 0.0f, 0.0f };

public:
	CObjectNosie();
	CObjectNosie(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 position, CShader* pShader =NULL);
	virtual ~CObjectNosie();
	
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);

	void SetAngle(XMFLOAT3 angle) { m_cbvNoisebuffer.Angle = angle; }
	void UpdateShaderNoiseBuffer(float Time, XMFLOAT3 scrollspeed, XMFLOAT3 scales, float padding);

	virtual void GenerateShaderDistortionBuffer();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

protected:
	ID3D12Resource                * m_pd3dPositionBuffer = NULL;
	ID3D12Resource                * m_pd3dPositionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	ID3D12Resource                * m_pd3dcbNoiseBuffer = NULL;
	CB_NOISEBUFFERTYPE            * m_pcbMappedNoiseBuffers = NULL;

	ID3D12Resource                * m_pd3dcbDistortionBuffer = NULL;
	CB_DISTORTIONBUFFERTYPE       * m_pcbMappdeDistortBuffers = NULL;

	ID3D12Resource                * m_pd3dRadiationLevel = NULL;
	int                           * m_iRadiationLevel = NULL;
;
};

//CObjectFog ===========================================================================================================
class CObjectFog : public CObjectNosie
{
public:
	CObjectFog(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 position, CShader* pShader = NULL);
	virtual ~CObjectFog();
	virtual void GenerateShaderDistortionBuffer();

};

//CObjectWater ===========================================================================================================
class CObjectWater : public CObjectNosie
{
public:
	CObjectWater(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature);
    virtual ~CObjectWater();

	virtual void NoiseSetTexture(ID3D12Device* pd3dDevice, 
		ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void GenerateShaderDistortionBuffer();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

};

