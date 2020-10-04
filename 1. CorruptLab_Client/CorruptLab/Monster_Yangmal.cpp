#include "Monster_Yangmal.h"
#include "Animation.h"
#include "Mgr_Collision.h"
#include "Object_Terrain.h"

void CYangmal::BadUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent, void* pContext)
{
	if (m_iState == MONSTER_STATE_STUN)
	{
		return;
	}

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	if (!m_bNotice) // 랜덤으로 돌아다니면서 쉬기
	{
		if (m_iState == MONSTER_STATE_IDLE)
		{
			SetAnimationSet(0, m_iTrackNumber); // idle
			m_fIdleTick += fTimeElapsed;
			if (m_fIdleTick >= 3)
			{
				m_fIdleTick = 0;
				m_iState = MONSTER_STATE_WALK;
				XMFLOAT3 randompos;
				randompos.x = float(rand() % 60) - 30.f;
				randompos.y = 0.0f;
				randompos.z = float(rand() % 60) - 30.f;
				m_xmf3RandomMoveDest = Vector3::Add(randompos, m_xmf3FiledCenter);
				if (pTerrain)
					m_xmf3RandomMoveDest.y = pTerrain->GetHeight(m_xmf3RandomMoveDest.x, m_xmf3RandomMoveDest.z);
			}
		}
		else if (m_iState == MONSTER_STATE_WALK || m_iState == MONSTER_STATE_RETURNING)
		{
			SetAnimationSet(3, m_iTrackNumber); // WALK
			MoveToTarget(m_xmf3RandomMoveDest, fTimeElapsed, m_fSpeed / 2.f, pTerrain);
			if (Vector3::Length(Vector3::Subtract(m_xmf3RandomMoveDest, GetPosition())) < 5.f)
			{
				m_iState = MONSTER_STATE_IDLE;
				if (isnan(m_xmf4x4World._41))
					int i = 0;
			}
		}
		if (m_fDistanceToPlayer < 70 && m_iState != MONSTER_STATE_RETURNING
			&& CCollisionMgr::GetInstance()->CheckMonsterNotice(0))
		{
			m_bNotice = true;
			m_iState = MONSTER_STATE_WALK;
		}
	}
	else
	{
		float yaw;
		switch (m_iState)
		{
		case MONSTER_STATE_WALK:
			yaw = MoveToTarget(m_xmf3PlayerPosition, fTimeElapsed, m_fSpeed * 0.4f, pTerrain);
			SetAnimationSet(3, m_iTrackNumber); // walk
			if (abs(yaw) < 0.3f) {
				m_iState = MONSTER_STATE_BACK;
				m_fIdleTick = 0;
			}
			break;
		case MONSTER_STATE_BACK:
			m_fIdleTick += fTimeElapsed;
			MoveToTarget(m_xmf3PlayerPosition, fTimeElapsed, m_fSpeed * -0.7f, pTerrain);
			SetAnimationSet(3, m_iTrackNumber); // walk
			if (m_fIdleTick > 1.f)
			{
				m_fIdleTick = 0;
				m_iState = MONSTER_STATE_ATTACK;
			}
			break;
		case MONSTER_STATE_ATTACK:
			Rush(m_xmf3PlayerPosition, fTimeElapsed, m_fSpeed * 1.1f, pTerrain);
			m_fIdleTick += fTimeElapsed;
			SetAnimationSet(2, m_iTrackNumber); // run
			if (m_fIdleTick > 3.f)
			{
				m_fIdleTick = 0;
				m_iState = MONSTER_STATE_WALK;
			}
			XMFLOAT4 Rotation = GetRotateQuaternion(m_xmf3Scale.x, m_pAttCollision->m_pParent->m_xmf4x4World);
			UpdateCollisionBoxes(pxmf4x4Parent, &Rotation, &m_xmf3Scale);
			m_pAttCollision->Update(pxmf4x4Parent, &Rotation, &m_xmf3Scale);
			CCollisionMgr::GetInstance()->MonsterAttackCheck(m_iAtt, *m_pAttCollision, fTimeElapsed);
			break;
		case MONSTER_STATE_DAMAGEING:
			SetAnimationSet(4, m_iTrackNumber);
			if (m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_pAnimationSet->m_fPosition >= 0.75f)
			{
				m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_pAnimationSet->m_fPosition = 0;
				m_iState = MONSTER_STATE_WALK;
			}
			break;
		default:
			break;
		}

		float fDistanceToFiled = Vector3::Length(Vector3::Subtract(m_xmf3FiledCenter, GetPosition()));

		if (m_fDistanceToPlayer > 60 || fDistanceToFiled > 80)
		{
			XMFLOAT3 randompos;
			randompos.x = float(rand() % 60) - 30.f;
			randompos.y = 0.0f;
			randompos.z = float(rand() % 60) - 30.f;
			m_xmf3RandomMoveDest = Vector3::Add(randompos, m_xmf3FiledCenter);
			m_iState = MONSTER_STATE_RETURNING;
			m_bNotice = false;
		}
	}
	if (GetDistanceToPlayer() < 7.f)
		SetPosition(m_xmf3PrePosition);

	m_xmf3PrePosition = GetPosition();
}
void CYangmal::GoodUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent, void* pContext)
{

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	if (m_iState == MONSTER_STATE_IDLE)
	{
		SetAnimationSet(0, m_iTrackNumber); // idle
		m_fIdleTick += fTimeElapsed;
		if (m_fIdleTick >= 2)
		{
			m_fIdleTick = 0;
			m_iState = MONSTER_STATE_WALK;
			XMFLOAT3 randompos;
			randompos.x = float(rand() % 60) - 30.f;
			randompos.y = 0.0f;
			randompos.z = float(rand() % 60) - 30.f;
			m_xmf3RandomMoveDest = Vector3::Add(randompos, m_xmf3FiledCenter);
			m_xmf3RandomMoveDest.y = pTerrain->GetHeight(m_xmf3RandomMoveDest.x, m_xmf3RandomMoveDest.z);
		}
	}
	else if (m_iState == MONSTER_STATE_WALK || m_iState == MONSTER_STATE_RETURNING)
	{
		SetAnimationSet(3, m_iTrackNumber); // WALK


		if (m_fDistanceToPlayer < 5.f) return;
		MoveToTarget(m_xmf3RandomMoveDest, fTimeElapsed, m_fSpeed / 2.f, pTerrain);
		if (Vector3::Length(Vector3::Subtract(m_xmf3RandomMoveDest, GetPosition())) < 3.f)
		{
			m_iState = MONSTER_STATE_IDLE;
			if (isnan(m_xmf4x4World._41))
				int i = 0;
		}
	}
}
void CYangmal::OnInitialize()
{

	m_pNeck = m_pChild->FindFrame("Bip001_Neck");
	if (m_pChild->m_pBoundingBoxes)
	{
		m_pAttCollision = &(m_pChild->m_pBoundingBoxes[1]);
		m_pAttCollision->m_pParent = m_pNeck;
	}

}

void CYangmal::Rush(XMFLOAT3& pos, float fTimeElapsed, float Speed, CHeightMapTerrain* pTerrain)
{
	XMFLOAT3 xmf3Position = GetPosition();

	if (m_fDistanceToPlayer < 7.f) return;

	XMFLOAT3 MovePos = Vector3::Add(xmf3Position, Vector3::ScalarProduct(GetLook(), Speed * fTimeElapsed));
	if (pTerrain)
		MovePos.y = pTerrain->GetHeight(MovePos.x, MovePos.z);

	SetPosition(MovePos);
}

void CYangmal::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent, int iNum)
{
	//SetAnimationSet(1);
	switch (m_iState)
	{
	case MONSTER_STATE_STUN:
		if (m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_fPosition >= m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_pAnimationSet->m_fLength - 0.1)
			m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_bEnable = false;
	default:
		CGameObject::Animate(fTimeElapsed, NULL,iNum);
		break;
	}
}
