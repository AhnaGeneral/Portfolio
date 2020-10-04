#include "Monster_Tosm.h"
#include "Object_Terrain.h"
#include "Animation.h"
#include "Mgr_Collision.h"
CTosm::CTosm()
{
	m_fSpeed = 20;
	m_iStunAnimation = 5;
}

void CTosm::Initialize(XMFLOAT3 FiledPos, int iAtt)
{
	m_fSpeed = 20;
	m_xmf3FiledCenter = FiledPos;
	m_iAtt = iAtt;
	m_pChild->m_pAnimationController->m_pAnimationSets[1].m_fSpeed /= 1.1f;
	m_pChild->m_pAnimationController->m_pAnimationSets[2].m_fSpeed /= 1.3f;
	m_pChild->m_pAnimationController->m_pAnimationSets[3].m_fSpeed /= 1.1f;
	

	m_pNeck = m_pChild->FindFrame("Bip001_Neck");
	if (m_pChild->m_pBoundingBoxes)
	{
		m_pAttCollision = &(m_pChild->m_pBoundingBoxes[0]);
		m_pAttCollision->m_pParent = m_pNeck;
	}

}

void CTosm::GoodUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent, void* pContext)
{

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	if (m_iState == MONSTER_STATE_IDLE)
	{
		SetAnimationSet(0, m_iTrackNumber); // idle
		m_fIdleTick += fTimeElapsed;
		if (m_fIdleTick >= m_fIdleTime)
		{
			m_fIdleTick = 0;
			m_iState = MONSTER_STATE_WALK;
			XMFLOAT3 randompos;
			randompos.x = float(rand() % 100) - 50.f;
			randompos.y = 0.0f;
			randompos.z = float(rand() % 100) - 50.f;
			m_xmf3RandomMoveDest = Vector3::Add(randompos, m_xmf3FiledCenter);
			m_xmf3RandomMoveDest.y = pTerrain->GetHeight(m_xmf3RandomMoveDest.x, m_xmf3RandomMoveDest.z);
		}
	}
	else if (m_iState == MONSTER_STATE_WALK || m_iState == MONSTER_STATE_RETURNING)
	{
		
		SetAnimationSet(1, m_iTrackNumber); // WALK
		MoveToTarget(m_xmf3RandomMoveDest, fTimeElapsed, m_fSpeed / 2.f, pTerrain);
		if (Vector3::Length(Vector3::Subtract(m_xmf3RandomMoveDest, GetPosition())) < 3.f)
		{
			m_fIdleTime = float(rand() % 30 + 10) / 10;
			m_iState = MONSTER_STATE_IDLE;
			if (isnan(m_xmf4x4World._41))
				int i = 0;
		}
	}

	if (GetDistanceToPlayer() < 7.f)
		SetPosition(m_xmf3PrePosition);

	m_xmf3PrePosition = GetPosition();
}

void CTosm::BadUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent, void* pContext)
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
			if (m_fIdleTick >= m_fIdleTime)
			{

				m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_fPosition = 0;
				m_fIdleTick = 0;
				m_iState = MONSTER_STATE_WALK;
				XMFLOAT3 randompos;
				randompos.x = float(rand() % 100) - 50.f;
				randompos.y = 0.0f;
				randompos.z = float(rand() % 100) - 50.f;
				m_xmf3RandomMoveDest = Vector3::Add(randompos, m_xmf3FiledCenter);
				if (pTerrain)
					m_xmf3RandomMoveDest.y = pTerrain->GetHeight(m_xmf3RandomMoveDest.x, m_xmf3RandomMoveDest.z);
			}
		}
		else if (m_iState == MONSTER_STATE_WALK || m_iState == MONSTER_STATE_RETURNING)
		{
			SetAnimationSet(1, m_iTrackNumber); // WALK
			MoveToTarget(m_xmf3RandomMoveDest, fTimeElapsed, m_fSpeed / 2.f, pTerrain);
			if (Vector3::Length(Vector3::Subtract(m_xmf3RandomMoveDest, GetPosition())) < 3.f)
			{
				m_fIdleTime = float(rand() % 30 + 10) / 10;
				m_iState = MONSTER_STATE_IDLE;
				if (isnan(m_xmf4x4World._41))
					int i = 0;
			}
		}
		if (m_fDistanceToPlayer < 70 && m_iState != MONSTER_STATE_RETURNING
			&& CCollisionMgr::GetInstance()->CheckMonsterNotice(1))
		{
			m_bNotice = true;
			m_iState = MONSTER_STATE_WALK;
		}
	}
	else
	{
		float fDistanceToFiled;
		float yaw;
		switch (m_iState)
		{
		case MONSTER_STATE_WALK:
			yaw = MoveToTarget(m_xmf3PlayerPosition, fTimeElapsed, m_fSpeed * 0.7f, pTerrain);
			SetAnimationSet(2, m_iTrackNumber); // RUN
			if (m_fDistanceToPlayer < 20) m_iState = MONSTER_STATE_ATTACK;


			fDistanceToFiled = Vector3::Length(Vector3::Subtract(m_xmf3FiledCenter, GetPosition()));

			if (m_fDistanceToPlayer > 70 || fDistanceToFiled > 100)
			{
				XMFLOAT3 randompos;
				randompos.x = float(rand() % 100) - 50.f;
				randompos.y = 0.0f;
				randompos.z = float(rand() % 100) - 50.f;
				m_xmf3RandomMoveDest = Vector3::Add(randompos, m_xmf3FiledCenter);
				m_iState = MONSTER_STATE_RETURNING;
				m_bNotice = false;
			}

			break;

		case MONSTER_STATE_ATTACK:
			yaw = MoveToTarget(m_xmf3PlayerPosition, fTimeElapsed, m_fSpeed * 0.7f, pTerrain);
		
			XMFLOAT4 Rotation = GetRotateQuaternion(m_xmf3Scale.x, m_pAttCollision->m_pParent->m_xmf4x4World);
			UpdateCollisionBoxes(pxmf4x4Parent, &Rotation, &m_xmf3Scale);
			m_pAttCollision->Update(pxmf4x4Parent, &Rotation, &m_xmf3Scale);
			CCollisionMgr::GetInstance()->MonsterAttackCheck(m_iAtt, *m_pAttCollision, fTimeElapsed);
			SetAnimationSet(3, m_iTrackNumber);
			if (m_fDistanceToPlayer > 20) {
				m_iState = MONSTER_STATE_WALK;
				m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_fPosition = 0;


				fDistanceToFiled = Vector3::Length(Vector3::Subtract(m_xmf3FiledCenter, GetPosition()));

				if (m_fDistanceToPlayer > 70 || fDistanceToFiled > 100)
				{
					XMFLOAT3 randompos;
					randompos.x = float(rand() % 100) - 50.f;
					randompos.y = 0.0f;
					randompos.z = float(rand() % 100) - 50.f;
					m_xmf3RandomMoveDest = Vector3::Add(randompos, m_xmf3FiledCenter);
					m_iState = MONSTER_STATE_RETURNING;
					m_bNotice = false;
				}
			}
			break;


		case MONSTER_STATE_DAMAGEING:
			SetAnimationSet(4, m_iTrackNumber);
			if (m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_pAnimationSet->m_fPosition >= 0.75f)
			{
				m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_pAnimationSet->m_fPosition = 0;
				m_iState = MONSTER_STATE_WALK;
			}
			break;
		}

	}

	if (GetDistanceToPlayer() < 7.f)
		SetPosition(m_xmf3PrePosition);

	m_xmf3PrePosition = GetPosition();
}

void CTosm::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent, int iNum)
{
	//SetAnimationSet(1);
	switch (m_iState)
	{
	case MONSTER_STATE_STUN:
		if (m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_fPosition >= m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_pAnimationSet->m_fLength - 0.05)
			m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_bEnable = false;
	default:
		CGameObject::Animate(fTimeElapsed, NULL, iNum);
		break;
	}
}