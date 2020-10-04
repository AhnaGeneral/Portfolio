#include "Mgr_Radiation.h"
#include "Object_Player.h"
//#include <random>

CRadationMgr* CRadationMgr::m_pInstance = NULL;

void CRadationMgr::Initialize()
{
	m_Radiations[0] = RadiationInfo(XMFLOAT2(389.f, 335.f), 200, 15);
	m_Radiations[1] = RadiationInfo(XMFLOAT2(357.f, 167.f), 100, 25);
	m_Radiations[2] = RadiationInfo(XMFLOAT2(194.f, 340.f), 170, 35);
	m_Radiations[3] = RadiationInfo(XMFLOAT2(203.f, 138.f), 150, 45);
	m_Radiations[4] = RadiationInfo(XMFLOAT2(101.f, 176.f), 100, 55);

	
}
void CRadationMgr::Update(float fElapsedTime)
{
	m_fTick += fElapsedTime;
	
	if (m_fTick > 0.7f)
	{
		m_iRadiation =GetStrength(m_pPlayer->GetPosition());
		m_fTick = 0.f; 
	}
}
int CRadationMgr::GetStrength(XMFLOAT3 pos)
{
	// 417,190 - 445,215
	XMFLOAT2 Pos = XMFLOAT2(pos.x, pos.z);
	if(Pos.x > 417 && Pos.x < 445 && Pos.y >190 && Pos.y <215)
			return 0;
	if (Pos.x > 204 && Pos.x < 240 && Pos.y >270 && Pos.y < 282)
		return 0;

	int Strength = 0;
	for (int i = 0; i < 5; ++i)
	{
		float distance = Vector2::Length(Vector2::Subtract(Pos, m_Radiations[i].Position));
		if (distance < m_Radiations[i].Range)
		{
			Strength = m_Radiations[i].Strength;
			break;
		}
	}
	if (Strength == 0)	Strength = 5;

	int rands = rand() % 10 + Strength;
	return rands;
	
}
void CRadationMgr::Destroy()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

RadiationInfo::RadiationInfo()
{
}
