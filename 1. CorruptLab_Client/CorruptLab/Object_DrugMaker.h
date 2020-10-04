#pragma once
#include "Object_DynamicObj.h"

class CDrugMaker : public CDynamicObject
{
public:
	CDrugMaker(int iMonsterType);

	bool		m_bEnable;
	int			m_iMonsterType;
};

