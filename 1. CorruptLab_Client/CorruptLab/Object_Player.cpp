//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Object_Player.h"
#include "Mgr_Scene.h"
#include "Mgr_Collision.h"
#include "Mgr_IndoorControl.h"
#include "SoundMgr.h"
#include "Animation.h"
#include "Shader.h"


// CPlayer ======================================================================================================
CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int nMeshes) : CGameObject()
{
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, -1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;
	m_xmf3Scale = XMFLOAT3(20.f, 20.f, 20.f);
	m_fComboTick = 0.0f;
	m_nCombo = 0;
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	CCollisionMgr::GetInstance()->SetPlayer(this);

	m_iMaxHP = 100.0f;
	m_iCurrentHP = 100.0f;
	m_iAtt = 5;

}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;

}

void CPlayer::SetType()
{
	m_iState = JOHNSON_ANIAMATION_TIPING;
	m_pSword->m_bRender = false;
	m_pChild->m_pAnimationController->m_pAnimationTracks->m_fPosition = 0;
}

void CPlayer::SetAttackState()
{
	m_bSwordEffectControl = true;
	if (m_bShift) m_nCombo = 1;
	else m_nCombo = 0;
	CSoundMgr::GetInstacne()->PlayEffectSound(_T("Attack"));
	
	m_iState = JOHNSON_ANIAMATION_ATTACK;
	m_pChild->m_pAnimationController->m_pAnimationTracks->m_fPosition = combolists[m_nCombo].Start;
	if (m_nCombo == 1)
		CCollisionMgr::GetInstance()->MonsterDamageCheck(100);
	else
		CCollisionMgr::GetInstance()->MonsterDamageCheck(m_iAtt);

	if (m_nCombo > 2) m_nCombo = 0;
	//SetAnima
}

void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();

	CGameObject::ReleaseShaderVariables();
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) {}


void CPlayer::UpdateCollisionBoxes(XMFLOAT4X4* world)
{
	OnPrepareRender();
	//if(m_pHandCollision) m_pHandCollision->Update(NULL);
	if(m_pBodyCollision) m_pBodyCollision->Update(&m_xmf4x4Transform,NULL, &m_xmf3Scale);
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (m_iState == JOHNSON_ANIAMATION_ATTACK || m_iState == JOHNSON_ANIAMATION_TIPING) return;
	if (dwDirection)
	{
		fDistance *= m_fSpeed;
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD)
		{
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		}
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);

		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Update(float fTimeElapsed)
{

	// ======================================================================
	if (m_iState == JOHNSON_ANIAMATION_ATTACK)
	{
		m_fComboTick += fTimeElapsed;
		SetAnimationSet(m_iState, m_iTrackNumber);
		if (m_fComboTick > combolists[m_nCombo].length)
		{
			m_fComboTick = 0;
 			m_iState = JOHNSON_ANIAMATION_IDLE;
			m_pChild->m_pAnimationController->m_pAnimationTracks->m_pAnimationSet->m_fPosition = 0;
		}
		return;
	}
	else if (m_iState == JOHNSON_ANIAMATION_TIPING)
	{
		m_fComboTick += fTimeElapsed;
		if (m_fComboTick > 8.5f)
		{
			m_pSword->m_bRender = true;
			m_iState = JOHNSON_ANIAMATION_IDLE;
			m_fComboTick = 0;
			if (CMgr_IndoorControl::GetInstance()->GetIsCoded(1) && CMgr_IndoorControl::GetInstance()->GetIsCoded(2))
				CMgr_IndoorControl::GetInstance()->m_bTurnToEnding = true;
		}
		SetAnimationSet(m_iState, m_iTrackNumber);
		return;
	}

	else
	{
		if (::IsZero(m_fVelocityXZ))
		{
			m_iState = JOHNSON_ANIAMATION_IDLE;
			m_fAcceleration = 0.2f; 
		}

		else if (!(::IsZero(m_fVelocityXZ)) && m_fAcceleration > 2.3f)
		{

			m_pChild->m_pAnimationController->m_pAnimationSets[JOHNSON_ANIAMATION_RUN].m_fSpeed = m_fAcceleration / 3.3f;
			m_iState = JOHNSON_ANIAMATION_RUN; 
		}
		else
		{
			m_iState = JOHNSON_ANIAMATION_WALK;
		}

		if (m_pPlayerUpdatedContext) 
		{
			if (m_fAcceleration < 3.0f && !(::IsZero(m_fVelocityXZ)))
			{
				m_fAcceleration += fTimeElapsed * (0.8f);
			}

			m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));

		}
		else
		{
			if (m_fAcceleration < 3.0f && !(::IsZero(m_fVelocityXZ)))
			{
				m_fAcceleration += fTimeElapsed * (0.8f);
			}
		}
	// ======================================================================

		float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);

		float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;

		if (fLength >= 2.f)
		{
			m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
			m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
		}

		float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;

		fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);

		if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

		XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, m_fAcceleration, false);
		Move(xmf3Velocity, false);

		if (m_pPlayerUpdatedContext)
			OnPlayerUpdateCallback(fTimeElapsed);
		else
		{
			XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
			SetVelocity(xmf3PlayerVelocity);

			XMFLOAT3 xmf3PlayerPosition = GetPosition();
			SetPosition(xmf3PlayerPosition);

		}

		SetAnimationSet(m_iState, m_iTrackNumber);

	}

	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	XMFLOAT3 pos = m_xmf3Position;
	pos.y += 9.3f;

	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(pos, fTimeElapsed);
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(pos);
	m_pCamera->RegenerateViewMatrix();

	m_fVelocityXZ = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);

	float fDeceleration = (m_fFriction * fTimeElapsed) * 0.5f;

	if (fDeceleration > m_fVelocityXZ)
		fDeceleration = m_fVelocityXZ;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));


}

CCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CPlayerCamera* pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, -1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4Transform._11 = m_xmf3Right.x; m_xmf4x4Transform._12 = m_xmf3Right.y; m_xmf4x4Transform._13 = m_xmf3Right.z;
	m_xmf4x4Transform._21 = m_xmf3Up.x; m_xmf4x4Transform._22 = m_xmf3Up.y; m_xmf4x4Transform._23 = m_xmf3Up.z;
	m_xmf4x4Transform._31 = m_xmf3Look.x; m_xmf4x4Transform._32 = m_xmf3Look.y; m_xmf4x4Transform._33 = m_xmf3Look.z;
	m_xmf4x4Transform._41 = m_xmf3Position.x; m_xmf4x4Transform._42 = m_xmf3Position.y; m_xmf4x4Transform._43 = m_xmf3Position.z;

	UpdateTransform(NULL);
}

void CPlayer::SetRootParameter(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);
}

void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CGameObject::Render(pd3dCommandList, pCamera, nPipelineState);
}


// CAirplanePlayer =====================================================================================================
CMainPlayer::CMainPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	 ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int nMeshes) 
	: CPlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pContext, nMeshes)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	m_pSword = NULL;

	CGameObject* pGameObject =
		CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			                  pd3dGraphicsRootSignature, "Model/Johnson/Johnson_Idle.bin", NULL, 1);

	m_pSword = pGameObject->FindFrame("Object001_1");

	//SetPosition(0.0f, 0.0f, 0.0f);

	m_pDummy = pGameObject->FindFrame("Dummy001");
	m_pBodyCollision = &(pGameObject->m_pBoundingBoxes[0]);

	//pGameObject->m_pBoundingBoxes[1].m_pParent = m_pRightHand;
	m_pHandCollision = &(pGameObject->m_pBoundingBoxes[1]);
	//pGameObject->m_xmf4Rotation

//	m_pSword->m_xmf4x4Transform = m_pDummy->m_xmf4x4Transform;
	//m_pSword->m_xmf4x4Transform = Matrix4x4::Identity();
	//m_pSword->SetScale(1, 2, 2);
	//m_pSword->Rotate(-90, 0, 180);

	SwordShader = new CSwordEffectShader();
	SwordShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	SwordShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SwordShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1,6); //16

	m_SwordEffectTexture = new CTexture(2, RESOURCE_TEXTURE2D, 0);
	m_SwordEffectTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Effect/Attack2.dds", 0);
	m_SwordEffectTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Effect/Attack1.dds", 1);

	SwordShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList,
		m_SwordEffectTexture, ROOT_PARAMETER_EFFECT, false);

	m_SwordEffect2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
		pd3dGraphicsRootSignature, "Effect/AttackAni2.bin", SwordShader, 0);
	m_SwordEffect2->SetScale(0.8f, 0.8f, 0.8f);
	m_SwordEffect2->Rotate(-90.f, 0.0f, 0.0f);

	m_SwordEffect = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
		pd3dGraphicsRootSignature, "Effect/AttackAni1.bin", SwordShader, 0);
	m_SwordEffect->SetScale(0.8f, 0.8f, 0.8f);
	m_SwordEffect->Rotate(-90.f, 0.0f, 0.0f);


	SetChild(pGameObject, true);

	OnInitialize();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	SetPlayerUpdatedContext(pTerrain);
	SetCameraUpdatedContext(pTerrain);
	
}

CMainPlayer::~CMainPlayer()
{

}

void CMainPlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CMainPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pCameraUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	int z = (int)(xmf3CameraPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z, bReverseQuad) + 4.0f;
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			CThirdPersonCamera* p3rdPersonCamera = (CThirdPersonCamera*)m_pCamera;
			p3rdPersonCamera->SetLookAt(XMFLOAT3(GetPosition().x, GetPosition().y + 5.0f, GetPosition().z));
		}
	}
}

void CMainPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{

	XMFLOAT3 xmf3PlayerPosition = GetPosition();

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pPlayerUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();

	int z = (int)(xmf3PlayerPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);

	if(CheckBridge(xmf3PlayerPosition) == false) return;
	
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z, bReverseQuad);
	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
		//m_xmf3Position = xmf3PlayerPosition;
	}

}

void CMainPlayer::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

	UINT getobjectID = m_ObjectID;
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 4, &getobjectID, 16);

	float SwordEffectTime = m_fSwordEffectTime;
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 4, &SwordEffectTime, 24);

}

bool CMainPlayer::CheckBridge(XMFLOAT3 xmf3PlayerPosition)
{
	if (xmf3PlayerPosition.x < 73)
	{
		if (xmf3PlayerPosition.z > 400)
		{
			if (xmf3PlayerPosition.x > 63) return true;
			if (xmf3PlayerPosition.x < -120)
			{
				SetVelocity(XMFLOAT3(0, 0, 0));
				CSceneMgr::GetInstance()->ChanegeSceneState(SCENE_STAGE_INDOOR);
				return false;
			}
			if (xmf3PlayerPosition.z > 455) xmf3PlayerPosition.z = 455;
			else if (xmf3PlayerPosition.z < 432) xmf3PlayerPosition.z = 432;

			float fY = xmf3PlayerPosition.x - (-120) - 90;
			fY = 30.0f + fY * fY * 0.003f;
			xmf3PlayerPosition.y = fY;
		}
		else if (xmf3PlayerPosition.z < 210)
		{
			if (xmf3PlayerPosition.x < -5)
			{
				xmf3PlayerPosition.x = -5;
			
			}
			if (xmf3PlayerPosition.z > 196) xmf3PlayerPosition.z = 196;
			else if (xmf3PlayerPosition.z < 173) xmf3PlayerPosition.z = 173;

			float fY = xmf3PlayerPosition.x - (-50) - 50;
			fY = 30.0f + fY * fY * 0.004f;
			xmf3PlayerPosition.y = fY;
		}
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		SetPosition(xmf3PlayerPosition);
		return false;
	}
	return true;
}

void CMainPlayer::Update(float fTimeElapsed)
{
	CPlayer::Update(fTimeElapsed);
	UpdateObjectID(0);
	UpdateCollisionBoxes();
	if (CCollisionMgr::GetInstance()->StaticCollisionCheck() ||
		CCollisionMgr::GetInstance()->DoorCollisionCheck())

		m_xmf3Position = m_xmf3PrePosition;
	m_xmf3PrePosition = m_xmf3Position;

	if (m_pSword && m_pDummy)
	{
		m_pSword->UpdateTransform(&m_pDummy->m_xmf4x4World);
	}

	if (m_SwordEffect || m_SwordEffect2)
	{
		XMFLOAT4X4 m_xmworld = m_xmf4x4World;
		m_SwordEffect->UpdateTransform(&m_xmworld);
		m_SwordEffect2->UpdateTransform(&m_xmworld);
	}
	if (m_bSwordEffectControl)
	{
		m_fSwordEffectRenderTime += fTimeElapsed;
		m_fSwordEffectTime += (fTimeElapsed * 3.0f);

		if (m_fSwordEffectTime > 1.0f)
		{
			m_fSwordEffectTime = 0.0f;
		}

		if (m_fSwordEffectRenderTime > 0.48f)
		{
			m_fSwordEffectRenderTime = 0.0f;
			m_bSwordEffectControl = false; 
		}
	}


	//SetAnimation();
}

void CMainPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CPlayer::Render(pd3dCommandList, pCamera, nPipelineState);

	if (m_bSwordEffectControl)
	{
		SwordShader->OnPrepareRender(pd3dCommandList, 0);
		if (m_SwordEffect && m_SwordEffect2)
		{
			if (m_fSwordEffectRenderTime >= 0.0f && m_fSwordEffectRenderTime < 0.2f)
			{
				m_SwordEffectTexture->UpdateShaderVariable(pd3dCommandList, 0);
				m_SwordEffect->Render(pd3dCommandList, pCamera, 0);
			}
			else 
			{
				m_SwordEffectTexture->UpdateShaderVariable(pd3dCommandList, 1);
				m_SwordEffect2->Render(pd3dCommandList, pCamera, 0);
			}
		}
	}
}

void CMainPlayer::SetAnimation()
{/*
	if (::IsZero(m_fVelocityXZ))
		SetAnimationSet(JOHNSON_ANIAMATION_IDLE);
	else if (m_fVelocityXZ >= 1.3f)
		SetAnimationSet(JOHNSON_ANIAMATION_RUN);
	else
		SetAnimationSet(JOHNSON_ANIAMATION_WALK);*/
}

CPlayerCamera* CMainPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(200.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = dynamic_cast<CPlayerCamera*>(OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode));
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;

	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = dynamic_cast<CPlayerCamera*>(OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode));
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;

	case THIRD_PERSON_CAMERA:
		SetFriction(300.0f);
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(150.f);
		SetMaxVelocityY(400.0f);
		m_pCamera = dynamic_cast<CPlayerCamera*>(OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode));
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 2.0f, -10.5f));
		m_pCamera->GenerateProjectionMatrix(1.01f, CAMERA_CULL_RANGE, ASPECT_RATIO, 50.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		//m_pCamera->GenerateOrthoLHMatrix(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.f);
		break;
	default:
		break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, XMFLOAT3(0.0f, 2.0f, 10.5f)));

	Update(fTimeElapsed);

	return(m_pCamera);
}

void CPlayer::GetDamaage(int iDamage)
{
	m_iCurrentHP -= iDamage;
	if (m_iCurrentHP <= 0) m_iCurrentHP = 0;
}
