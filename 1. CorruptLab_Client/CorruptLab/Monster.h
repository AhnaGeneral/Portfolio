#pragma once
#include "Object_StaticObj.h"
#include "Object_Nosie.h"

class CUI_MonsterHP;
class CHeightMapTerrain;


/*
몬스터의 행동 패턴
1.플레이어를 알아차리지 못했을때는 랜덤하게 돌아다니며 쉰다
	- 랜덤으로 돌아다니는 위치는 필드의 중심점을 일정 이상 벗어나지 않는다.
2. 플레이어와의 거리가 70 이하이면 플레이어를 따라온다.
	 - 플레이어와의 거리가 40 이상이면 뛰어오며 공격한다.
	 - 플레이어와의 거리가 40 미만이면 걸어온다.
3. 플레이어에게 피격당하면 DAMAGED 상태가 된다.
	- DAMAGED 애니메이션이 끝날때까지 공격받거나 공격판정하지 않는다.
4. 체력이 20 이하가 되면 STUN 상태가 된다.
	- STUN 애니메이션은 한번만 재생된다.
5. 플레이어와의 거리가 70 이상이거나 필드의 중심점보다 100 이상 멀어지면 1번 상태로 돌아간다.
	- RETURNING 상태로 지정되며 이때는 플레이어를 인식하지 않는다.
	- 하지만 도중에 피격되면 다시 플레이어를 인식한다.
*/

class CMonster : public CGameObject
{
public:
	 CMonster();
	virtual ~CMonster();

protected:
	//GameContents

	CB_NOISEBUFFERTYPE* m_pcbMappedNoiseBuffers = NULL;
	float			m_fDissolveTime = 0;
	ID3D12Resource* m_pd3dDissolveTime = NULL;
	float			m_fFurifyingTime = 0;
	int				m_iStunAnimation = 1;
	float			m_fHarfHeight = 0 ;
	XMFLOAT3		m_xmf3PrePosition = XMFLOAT3(0, 0, 0);
	CAnimationController* m_pAnimationController = NULL;
	int				m_iMaxHP;
	int				m_iCurrentHP;
	int				m_iAtt ;
	float			m_fDistanceToPlayer;
	float			m_fSpeed;
	float			m_fIdleTick;
	bool			m_bIsPurified;
	XMFLOAT3		m_xmf3PlayerPosition;
	XMFLOAT3		m_xmf3RandomMoveDest;
	XMFLOAT3		m_xmf3FiledCenter;

	CCollisionBox*	m_pAttCollision;
	CCollisionBox*  m_pBodyCollision;
	CUI_MonsterHP*  m_HPUI;
	///
public:

	bool			m_bNotice;
	virtual void Initialize(XMFLOAT3 FiledPos, int iAtt);
	bool		IsPurified() { return m_bIsPurified; }
	int				m_iState = MONSTER_STATE_IDLE;
	//GameContents
	virtual float MoveToTarget(XMFLOAT3& pos, float fTimeElapsed, float Speed, CHeightMapTerrain* pTerrain = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int nPipelineState = 0);
	inline void SetDistanceToPlayer(const float& distance) { m_fDistanceToPlayer = distance; }
	inline float GetDistanceToPlayer() { return m_fDistanceToPlayer; }
	inline void SetPlayerPosition(const XMFLOAT3& pos) { m_xmf3PlayerPosition = pos; }
	void SetHPUI(CUI_MonsterHP* pHP);
	virtual void Update(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL, void* pContext = NULL);
	virtual void BadUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL, void* pContext = NULL);
	virtual void GoodUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL, void* pContext = NULL);
	virtual void PurifyingUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL, void* pContext = NULL);

	virtual void GetPurified();
	
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);


	virtual void OnInitialize();
	void GetDamaage(int iDamage);
	void GetFullHP() { m_iCurrentHP = m_iMaxHP; }
	///

};

