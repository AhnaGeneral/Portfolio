#pragma once
#include "Object_DynamicObj.h"

#define RESEARCHER_IDLE 0 
#define RESEARCHER_WARNING 1
#define RESEARCHER_BEG 1


class CResearcher :
    public CDynamicObject
{
public:
    CResearcher(int iFiled) { m_iFiledNumber = iFiled; }
    ~CResearcher() {}

    virtual void Update(float fElapsedTime);
    virtual void OnInitialize();

    void SetBilBil() { m_iState = RESEARCHER_BEG; }

    int m_iFiledNumber;
    int m_iState = RESEARCHER_IDLE;
};

