#pragma once

#include	"Object_DynamicObj.h"
class CDesk : public CDynamicObject
{
public:
	CDesk() {}
	~CDesk() {}

	virtual void OnInitialize();
	virtual void Update(float fElapsedTime);

	void Open();

private:
	bool m_bAnimating = false;
	bool m_bIsFull = true;
};

