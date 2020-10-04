
#include "Object_Door.h"
#include "Animation.h"

void CDoor::OnInitialize()
{
	CDynamicObject::OnInitialize();
	m_pChild->m_pAnimationController->m_pAnimationTracks[0].m_pAnimationSet->m_nType = ANIMATION_TYPE_ONCE;
	m_pChild->m_pAnimationController->m_pAnimationTracks[0].m_pAnimationSet->m_fSpeed /= 2.f;
}

void CDoor::Update(float fElapsedTime)
{
	if (m_bAnimating)
	{
		SetAnimationSet(0, m_iTrackNumber);
		Animate(fElapsedTime, NULL, m_iTrackNumber);
		if (m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_fPosition >= 4.0f)
		{
			m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_bEnable = false;
			m_bAnimating = false;
		}
	}
	else
		UpdateAnimationCache(m_iTrackNumber);
}

void CDoor::Open()
{
	m_bAnimating = true;
	m_bIsFull = true;
}