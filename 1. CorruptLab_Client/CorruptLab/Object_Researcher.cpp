#include "Object_Researcher.h"
#include "Animation.h"

void CResearcher::Update(float fElapsedTime)
{
	if(m_iState == RESEARCHER_IDLE)
		SetAnimationSet(0, m_iTrackNumber);
	else
		SetAnimationSet(1, m_iTrackNumber);

	Animate(fElapsedTime,NULL,m_iTrackNumber);
}

void CResearcher::OnInitialize()
{
	CDynamicObject::OnInitialize();
	m_pChild->m_pAnimationController->m_pAnimationSets[1].m_fSpeed /= 2.f;
}
