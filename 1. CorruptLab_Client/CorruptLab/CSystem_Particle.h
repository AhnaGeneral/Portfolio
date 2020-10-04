#pragma once
#include "Object.h"

struct ParticleType
{
	float posX, posY, PosZ; 
	float red, green, blue; 
	float velocity;
	float size;
	bool active; 
};

struct CB_ParicleShader
{
	XMFLOAT3 pos; 
	float size;
	XMFLOAT4 color; 
};

class ParticleSystemObject : public CGameObject
{
private:
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology 
		             = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float m_particleDeviationX = 0; //Deviation ¿œ≈ª, ∆Ì¬˜
	float m_particleDeviationY = 0;
	float m_particleDeviationZ = 0;
	float m_ParticleVelocity = 0; 
	float m_ParticleVelocityVariation = 0; //Variation ∫Ø»≠
	float m_ParticleSize = 0;
	float m_ParticlePerSecond = 0; 
	int m_maxParticles = 0; 
	float m_DeadIineY = 50.0f;
	int m_CurrentParticleCount = 0; 
	float m_accumulatedTime = 0; //accumulate √‡√¥


	CTexture* m_Texture = nullptr; 

	ParticleType* m_ParticleList = nullptr;
	XMFLOAT3 m_xmColor = XMFLOAT3(0.0f, 0.0f, 0.0f);
	bool m_ColorSwitch = false;

	CB_ParicleShader* m_Vertices = nullptr; 

	bool m_bEnable = false;
	// ==================================================
	int m_vertexCount = 0; 
	ID3D12Resource            * m_pd3dcbVertexBuffer = nullptr;
	ID3D12Resource            * m_pd3dVertexUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW    m_d3dVertexBufferView;
	CB_ParicleShader                * m_pd3dVertices = nullptr;

	XMFLOAT3                    m_EmplacePos = XMFLOAT3(0.0f, 0.0f, 0.0f); 
	// ==================================================

public:
	ParticleSystemObject() {}
	ParticleSystemObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 _EmplacePos = XMFLOAT3(0.0f, 0.0f,0.0f), float _DeadLineY = 50.0f);
	virtual ~ParticleSystemObject();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();
     void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);

public:
	void SwitchEnable();
	void RegenerateParticles(XMFLOAT3 Emitpos);
	void InitializeBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	bool InitializeParticleSystem(float velocity , float ParticlePerSecond, float maxParticles); 
	void EmitParticles(float frameTime); 
	void SetColor(float _red, float _green, float _blue);
	void UpdateParticles(float frameTime);
	void KillParticles();
	void DisconnectList(); 
	void CreateParticleShaderTexture(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, wchar_t* m_filename = nullptr);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);

	void Shutdown(); 
	void Frame(ID3D12Device* pd3dDevice, float frameTime, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

};