#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#include "Object.h"
#include "Object_Terrain.h"
#include "Object_Camera.h"
#include "Shader_Standard.h"

struct AttackCombo
{
	float Start;
	float length;
	AttackCombo(float s, float l) { Start = s; length = l; }
};



class CPlayer : public CGameObject
{
protected:
	XMFLOAT3			   m_xmf3PrePosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3	           m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3	           m_xmf3Right = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	XMFLOAT3	           m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3	           m_xmf3Look = XMFLOAT3(0.0f, 0.0f, -1.0f);

	XMFLOAT3	           m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3               m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float				   m_fSpeed = 10;
	float                  m_fPitch = 0.0f;
	float                  m_fYaw = 0.0f;
	float                  m_fRoll = 0.0f;

	float                  m_fAcceleration = 0.5f; 
	float				   m_fVelocityXZ = 0.0f;
	float                  m_fMaxVelocityXZ = 0.0f;
	float                  m_fMaxVelocityY = 0.0f;
	float                  m_fFriction = 0.0f;

	LPVOID		           m_pPlayerUpdatedContext = NULL;
	LPVOID		           m_pCameraUpdatedContext = NULL;

	CPlayerCamera        * m_pCamera = NULL;
	CGameObject          * m_pDummy = NULL;
	CGameObject          * m_pSword = NULL;

	CGameObject* m_SwordEffect = NULL;
	CGameObject* m_SwordEffect2 = NULL;
	CTexture* m_SwordEffectTexture = NULL;
	CShader* SwordShader = NULL;

	int					   m_nRightHandIndex = 25;

//GameContents
	int					   m_iState;
	float				   m_iMaxHP ;
	float			       m_iCurrentHP ;
	float				   m_fComboTick;
	int					   m_nCombo;
	int				       m_iAtt ;
	bool                   m_bSwordEffectControl = false; 

///
public:

//GameContents
	void GetDamaage(int iDamage); 
	void GetFullHP() { m_iCurrentHP = m_iMaxHP; }
	float* GetPlayerHPPointer() { return &m_iCurrentHP; }
///
	AttackCombo combolists[2] =
	{
		AttackCombo(0,0.4f), AttackCombo(0.4f,1.7f),
	};
	bool					m_bShift = false;

	CCollisionBox* m_pHandCollision;
	CCollisionBox* m_pBodyCollision;

	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, int nMeshes = 1);
	virtual ~CPlayer();

	XMFLOAT3* GetPositionPointer() { return(&m_xmf3Position); }
	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	void SetType();
	void SetAttackState();
	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }

	virtual void UpdateCollisionBoxes(XMFLOAT4X4* world = NULL);

	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	CCamera* GetCamera() { if (m_pCamera) return(m_pCamera); else return NULL; }
	void SetCamera(CPlayerCamera* pCamera) { m_pCamera = pCamera; }
	void Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	void Rotate(float x, float y, float z);

	virtual void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) {}
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) {}
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual CPlayerCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }
	virtual void OnPrepareRender();
	virtual void SetRootParameter(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);

};

class CMainPlayer : public CPlayer
{
public:
	CMainPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, int nMeshes = 1);
	virtual ~CMainPlayer();
	
	virtual CPlayerCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void OnCameraUpdateCallback(float fTimeElapsed);
	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	//virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);

	bool CheckBridge(XMFLOAT3 xmf3PlayerPosition);
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);

	void SetAnimation();
};

