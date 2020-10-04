
#include "Mgr_Collision.h"
#include "Object_Player.h"
#include "Monster.h"
#include "SoundMgr.h"
#include "Mgr_Item.h"

CCollisionMgr* CCollisionMgr::m_pInstance = NULL;

void CCollisionMgr::Initialize(void)
{
	m_pFiledCollision1.Center = XMFLOAT3(300.f, 65.f, 256.f);
	m_pFiledCollision1.Extents = XMFLOAT3(30, 20, 510.f);

	m_pFiledCollision2[0].Center = XMFLOAT3(160.f, 70.f, 230.f);
	m_pFiledCollision2[0].Extents = XMFLOAT3(130.0f, 30, 5.f);

	m_pFiledCollision2[1].Center = XMFLOAT3(340.f, 70.f, 380.f);
	m_pFiledCollision2[1].Extents = XMFLOAT3(5.f, 20.f, 140.f);

	FiledPos[0] = XMFLOAT3(377, 39, 118);
	FiledPos[1] = XMFLOAT3(206, 83, 138);
}

bool CCollisionMgr::CheckMonsterNotice(int iType)
{
	int notice = 0;
	for (auto p : *m_pMonsterLists[iType])
	{
		if (p->m_bNotice && p->m_iState != MONSTER_STATE_STUN) notice++;
	}
	if (notice <= 1) return true;
	else return false;
}

void CCollisionMgr::InsertCollisionBoxes(BoundingOrientedBox box)
{
	m_pStaticCollisionlist[m_nSceneState].push_back(box);
}

bool CCollisionMgr::StaticCollisionCheck()
{
	CCollisionBox* playerBodybox = m_pPlayer->m_pBodyCollision;
	if (!playerBodybox) return false;
	for (auto objCol : m_pStaticCollisionlist[m_nSceneState])
	{
		if (objCol.Intersects(playerBodybox->boundingBox))
			return true;
	}
	

	switch (m_iSceneProgress)
	{
	case PROGRESS_FILED1:
		if (m_pFiledCollision1.Intersects(playerBodybox->boundingBox))
			return true;
		break;
	//case PROGRESS_FILED2:
	//	for (int i = 0; i < 2; i++)
	//	{
	//		if (m_pFiledCollision2[i].Intersects(playerBodybox->boundingBox))
	//			return true;
	//	}
	//	break;
	default:
		break;
	}
	return false;
}

void CCollisionMgr::MonsterAttackCheck(int iDamaege, CCollisionBox box, float fTimeElapsed)
{
	CCollisionBox* playerBodybox = m_pPlayer->m_pBodyCollision;
	m_fTime += fTimeElapsed;

	if (box.boundingBox.Intersects(playerBodybox->boundingBox))
	{
		m_pPlayer->UpdateObjectID(1);
		if (m_fTime > 2.f)
		{
			CSoundMgr::GetInstacne()->PlayEffectSound(_T("Attacked"));
			m_pPlayer->GetDamaage(iDamaege);
			m_fTime = 0.0f;
		}
	}
}

void CCollisionMgr::MonsterDamageCheck(int iDamage)
{
	if (iDamage == 100 && m_pMonsterLists) {
		XMFLOAT3 playerPos = m_pPlayer->GetPosition();
		float distance = Vector3::Length(Vector3::Subtract(playerPos, FiledPos[m_iSceneProgress - 1]));
		if (distance > 50) return;

		for (auto Obj : *m_pMonsterLists[m_iSceneProgress - 1])
		{
			if (!Obj->m_bRender || Obj->m_iState == MONSTER_STATE_STUN || Obj->m_iState == MONSTER_STATE_DAMAGEING) continue;

				Obj->GetDamaage(iDamage);
		}
		return;
	}
	if (m_pMonsterLists) // ∏ÛΩ∫≈Õ Render
	{
		for (int i = 0; i < 2; i++)
		{
			for (auto Obj : *m_pMonsterLists[i])
			{
				if (!Obj->m_bRender || Obj->m_iState == MONSTER_STATE_STUN || Obj->m_iState == MONSTER_STATE_DAMAGEING) continue;
				
				if (Obj->GetDistanceToPlayer() < 12)
				{


					Obj->GetDamaage(iDamage);
				}
			}
		}
	}

}

bool CCollisionMgr::DoorCollisionCheck()
{
	CCollisionBox* playerBodybox = m_pPlayer->m_pBodyCollision;
	if (!playerBodybox) return false;
	for (int i = 0; i < 2; i++)
	{
			XMFLOAT4 Rotation = m_pOpeningDoors[i]->
			GetRotateQuaternion(m_pOpeningDoors[i]->m_xmf3Scale.y,
				m_pOpeningDoors[i]->m_xmf4x4World);

			m_pOpeningDoors[i]->UpdateCollisionBoxes(
				&m_pOpeningDoors[i]->m_xmf4x4World, &Rotation, &m_pOpeningDoors[i]->m_xmf3Scale);

			CCollisionBox* boxes = m_pOpeningDoors[i]->GetCollisionBoxes();
			BoundingOrientedBox objCol = boxes[0].boundingBox;
			BoundingOrientedBox objCol2 = boxes[1].boundingBox;
			if (objCol.Intersects(playerBodybox->boundingBox)  || objCol2.Intersects(playerBodybox->boundingBox))
				return true;
	}
	return false;
}