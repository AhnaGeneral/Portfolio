
#include "Mgr_Collision.h"
#include "Object_StaticObj.h"

void CStaticObject::OnInitialize()
{
	for (int i = 0; i < m_pChild->m_nBoundingBoxes; i++)
	{
		m_pChild->m_pBoundingBoxes[i].Update(&m_xmf4x4World, &m_xmf4Rotation, &m_xmf3Scale);
		BoundingOrientedBox box = m_pChild->m_pBoundingBoxes[i].boundingBox;
		CCollisionMgr::GetInstance()->InsertCollisionBoxes(box);
	}
}
