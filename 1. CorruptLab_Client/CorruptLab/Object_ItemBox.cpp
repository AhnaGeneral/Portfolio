#include "Object_ItemBox.h"
#include "Animation.h"
#include "Mgr_Item.h"
void CItemBox::OnInitialize()
{
	CDynamicObject::OnInitialize();
	m_pChild->m_pAnimationController->m_pAnimationTracks[0].m_pAnimationSet->m_nType = ANIMATION_TYPE_ONCE;
	m_pChild->m_pAnimationController->m_pAnimationTracks[0].m_pAnimationSet->m_fSpeed /= 3.f;
}

void CItemBox::Update(float fElapsedTime)
{
	if (m_bAnimating)
	{
		SetAnimationSet(0, m_iTrackNumber);
		Animate(fElapsedTime, NULL, m_iTrackNumber);
		if (m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_fPosition >= 0.4f)
		{
			m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_bEnable = false;
			m_bAnimating = false;

			if (m_itemunber > 2)
			{
				m_itemunber = 0;
			}
			CItemMgr::GetInstance()->GetItem(m_itemunber, GetPosition());
			m_itemunber++; 

		}
	}
	else
		UpdateAnimationCache(m_iTrackNumber);
}

void CItemBox::Open()
{
	m_bAnimating = true;
	m_bIsFull = true;
}
