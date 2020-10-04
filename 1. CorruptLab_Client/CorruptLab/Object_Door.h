#pragma once
#include	"Object_DynamicObj.h"
class CDoor : public CDynamicObject
{
public:
	CDoor() {}
	~CDoor() {}

	virtual void OnInitialize();
	virtual void Update(float fElapsedTime);

	void Open();

private:
	bool m_bAnimating = false;
	bool m_bIsFull = true;
};

