#include "Mgr_IndoorControl.h"
#include "Scene_Game2.h"
#include "Shader_LightMRT.h"
#include "SoundMgr.h"
#include "CNarrationMgr.h"

CMgr_IndoorControl* CMgr_IndoorControl::m_pInstance = NULL;

void CMgr_IndoorControl::Initialize()
{
	listpassword = new std::list<int>; 
}

void CMgr_IndoorControl::Update(float fElapsedTime)
{
}

int CMgr_IndoorControl::GetPasswordControl()
{
	return passwordControl;
}

void CMgr_IndoorControl::SetpasswordControl(int _passwordControl)
{
	passwordControl = _passwordControl; 
}

std::list<int>* CMgr_IndoorControl::GetlistPassword()
{
	return listpassword;
}

void CMgr_IndoorControl::InsertPassword(int i)
{
	if(listpassword->size() < 4)
		listpassword->push_back(i);
}

void CMgr_IndoorControl::EraserPassword()
{
	if (!listpassword->empty())
		listpassword->pop_back();
}

bool CMgr_IndoorControl::ConfirmDoorPassword()
{
	if (listpassword->size() != 4) return false; 
	else
	{
		int i = 0;
		for (auto& a : *listpassword)
		if (m_DoorPassWord[i++] != a)
		{
			CSoundMgr::GetInstacne()->PlayEffectSound(_T("WrongNumber"));
			return false;
		}
		m_Coded[0] = true;
		CSoundMgr::GetInstacne()->PlayEffectSound(_T("RightNumber"));
		CSoundMgr::GetInstacne()->PlayEffectSound(_T("door2"));
		return true;
	}
}

bool CMgr_IndoorControl::ConfirmRightPassword()
{
	if (listpassword->size() != 4) return false;
	else
	{
		int i = 0;
		for (auto& a : *listpassword)
			if (m_RightPassWord[i++] != a)
			{
				CSoundMgr::GetInstacne()->PlayEffectSound(_T("WrongNumber"));
				return false;
			}
		CSoundMgr::GetInstacne()->PlayEffectSound(_T("RightNumber"));
		m_Coded[2] = true;
		SetpasswordControl(false);
		pIndoorScene->CodeMachine(true);
		listpassword->clear();
		return true;
	}
}

bool CMgr_IndoorControl::ConfirmLeftPassword()
{
	if (listpassword->size() != 4) return false;
	else
	{
		int i = 0;
		for (auto& a : *listpassword)
			if (m_LeftPassWord[i++] != a)
			{
				CSoundMgr::GetInstacne()->PlayEffectSound(_T("WrongNumber"));
				return false;
			}
		CSoundMgr::GetInstacne()->PlayEffectSound(_T("RightNumber"));
		m_Coded[1] = true;
		SetpasswordControl(false);
		pIndoorScene->CodeMachine(false);
		listpassword->clear();
		return true;
	}
}

void CMgr_IndoorControl::SetDeskOpenControl(int _DeskOpeningControl)
{
	DeskOpenControl = _DeskOpeningControl;
	pIndoorScene->ExcuteAnimationDesk(DeskOpenControl);
}

int CMgr_IndoorControl::GetDeskOpenControl()
{
	return DeskOpenControl;
}

void CMgr_IndoorControl::SetThatIsRightPassword(bool _ThatIsRight)
{
	ThatIsRightPassword = _ThatIsRight; 
	SetExecuteConfirmControl(true);
}

void CMgr_IndoorControl::SetExecuteConfirmControl(bool _Excute)
{
	ExecuteConfirmControl = _Excute; 
}

bool CMgr_IndoorControl::GetThatIsRightPassword()
{
	return ThatIsRightPassword;
}

bool CMgr_IndoorControl::GetExecuteConfirmControl()
{
	return ExecuteConfirmControl;
}

void CMgr_IndoorControl::SetExecuteHandLightControl(bool _Excute)
{
	ExecuteHandLightControl = _Excute; 
}

bool CMgr_IndoorControl::GetExecuteHandLightControl()
{
	return ExecuteHandLightControl;
}

void CMgr_IndoorControl::SetIndoorSceneAndLight(CGameScene2* _IndoorScene, CLightTarget* _LightTarget)
{
	pIndoorScene = _IndoorScene;
	pIndoorLightTarget = _LightTarget;
}

void CMgr_IndoorControl::ExcuteAnimationDoor()
{
	pIndoorScene->ExcuteAnimationDoor();
	pIndoorScene->IndoorParticleEffectRender();
	pIndoorScene->SetThatIsRightPassword(true); 
	pIndoorLightTarget->TurnOnLabatoryLight();
	
}


void CMgr_IndoorControl::Destroy()
{
}
