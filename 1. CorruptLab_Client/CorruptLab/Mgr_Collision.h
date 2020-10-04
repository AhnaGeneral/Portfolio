#pragma once

#include "stdafx.h"
using namespace std;

class CCollisionBox;
class CPlayer;
class CMonster;
class CGameObject;


class CCollisionMgr
{
public:
	CCollisionMgr() {}
	~CCollisionMgr() {}

private:
	static CCollisionMgr* m_pInstance ;
	CPlayer* m_pPlayer = NULL;
	float m_fTime = 0.5f;

public:
	void Initialize(void);
	static CCollisionMgr* GetInstance(void)
	{
		if (m_pInstance == NULL)
			m_pInstance = new CCollisionMgr();
		return m_pInstance;
	}
	void Destroy()
	{
		if (m_pInstance)
		{
			delete m_pInstance; 
			m_pInstance = NULL;
		}
	}

	bool CheckMonsterNotice(int iType);
	void InsertCollisionBoxes(BoundingOrientedBox box);
	void SetPlayer(CPlayer* player) { m_pPlayer = player;  }

	bool StaticCollisionCheck();
	bool DoorCollisionCheck();
	void MonsterAttackCheck(int iDamaege, CCollisionBox box, float fTimeElapsed);
	void MonsterDamageCheck(int iDamage);
		
public:
	int m_nSceneState = 0;
	vector<BoundingOrientedBox> m_pStaticCollisionlist[2];
	vector<CMonster*>** m_pMonsterLists;
	int m_iSceneProgress = PROGRESS_FILED1;
	XMFLOAT3 FiledPos[2] ;
	BoundingBox m_pFiledCollision1;
	BoundingBox m_pFiledCollision2[2];
	CGameObject* m_pOpeningDoors[2];

};

