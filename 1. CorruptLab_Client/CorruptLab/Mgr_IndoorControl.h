#pragma once
#include "stdafx.h"
#include <iostream>
#include <list>


class CGameScene2; 
class CGameFramework;
class CLightTarget; 

class CMgr_IndoorControl
{
private:
	int passwordControl = 0; 
	int DeskOpenControl = 0;
	const int m_DoorPassWord[4] = { 8,2,7,7 };
	const int m_LeftPassWord[4] = { 1,9,9,6};
	const int m_RightPassWord[4] = { 1,9,9,6 };
	bool	m_Coded[3] = {false, false, false }; // 0 : door, 1 : Left, 2 : right

	std::list<int>* listpassword = nullptr;
	static CMgr_IndoorControl* m_pInstance;

	bool ThatIsRightPassword = false; 
	bool ExecuteConfirmControl = false; 
	bool ExecuteHandLightControl = false; 

	CGameScene2  * pIndoorScene = nullptr;
	CLightTarget * pIndoorLightTarget = nullptr;

public:
	bool m_bTurnToEnding = false;
	bool GetIsCoded(int index) { return m_Coded[index]; }
	void Initialize();
	void Update(float fElapsedTime);

	std::list<int>* GetlistPassword();

	void InsertPassword(int i);
	void EraserPassword();
	bool ConfirmDoorPassword(); 
	bool ConfirmRightPassword();
	bool ConfirmLeftPassword();

	void SetDeskOpenControl(int _DeskOpeningControl);
	int  GetDeskOpenControl();

	void SetpasswordControl(int _passwordControl);
	int  GetPasswordControl();

	void SetThatIsRightPassword(bool _ThatIsRight); 
	bool GetThatIsRightPassword();

	void SetExecuteConfirmControl(bool _Excute);
	bool GetExecuteConfirmControl();

	void SetExecuteHandLightControl(bool _Excute);
	bool GetExecuteHandLightControl();

	void SetIndoorSceneAndLight(CGameScene2* _IndoorScene, CLightTarget* _LightTarget);
	void ExcuteAnimationDoor();

	
	
	static CMgr_IndoorControl* GetInstance(void)
	{
		if (m_pInstance == NULL)
			m_pInstance = new CMgr_IndoorControl();
		return m_pInstance;
	}
	void Destroy();
};