#pragma once
#include	"Object_DynamicObj.h"
class CItemBox : public CDynamicObject
{
public:
	CItemBox() {}
	~CItemBox() {}

	virtual void OnInitialize();
	virtual void Update(float fElapsedTime);

	void Open();

private:
	bool m_bAnimating = false;
	bool m_bIsFull = true;
	int  m_itemunber = 0;
};

