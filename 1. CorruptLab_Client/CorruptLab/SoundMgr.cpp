#include "stdafx.h"
#include "SoundMgr.h"

CSoundMgr* CSoundMgr::m_pInstance = NULL;

CSoundMgr::CSoundMgr(void)
{
	m_pSystem	= NULL;

	m_pEffect	= NULL;	
	m_pMainBGM	= NULL;
	m_pSkill	= NULL;	
	m_pWater	= NULL;

	m_iVersion = 0;

	m_Result = FMOD_OK;

	Initialize();
}

CSoundMgr::~CSoundMgr(void)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter     = m_mapSound.begin();
	map<TCHAR*, FMOD::Sound*>::iterator iter_end = m_mapSound.end();
	
	for(iter; iter != iter_end; )
	{
		m_Result = iter->second->release();
		ErrorCheck(m_Result);

		delete [] (iter->first);

		iter = m_mapSound.erase(iter);
		iter_end = m_mapSound.end();

		if(iter == iter_end)
			break;
	}

	m_Result = m_pSystem->close();
	ErrorCheck(m_Result);

	m_Result = m_pSystem->release();
	ErrorCheck(m_Result);
}

void CSoundMgr::Initialize(void)
{

	m_Result = FMOD::System_Create(&m_pSystem);
	ErrorCheck(m_Result);

	m_Result = m_pSystem->getVersion(&m_iVersion);
	ErrorCheck(m_Result);

	m_pSystem->init(32, FMOD_INIT_NORMAL, NULL);
	ErrorCheck(m_Result);
	m_pMainBGM->setVolume(0.7f);
}

void CSoundMgr::LoadSoundFile(void)
{


	FMOD::Sound* pSound;
	m_Result = m_pSystem->createSound("Sounds/BGM/Logo_BGM.mp3", FMOD_LOOP_NORMAL, 0, &pSound);
	TCHAR* pName = _T("LogoBGM");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}

	m_Result = m_pSystem->createSound("Sounds/BGM/Boss_BGM.mp3", FMOD_LOOP_NORMAL, 0, &pSound);
	pName = _T("BossBGM");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}


	m_Result = m_pSystem->createSound("Sounds/BGM/Ending_BGM.mp3", FMOD_LOOP_NORMAL, 0, &pSound);
	pName = _T("EndingBGM");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}

	m_Result = m_pSystem->createSound("Sounds/BGM/Stage1_BGM.mp3", FMOD_LOOP_NORMAL, 0, &pSound);
	pName = _T("Stage1BGM");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}

	m_Result = m_pSystem->createSound("Sounds/BGM/Stage2_BGM.mp3", FMOD_LOOP_NORMAL, 0, &pSound);
	pName = _T("Stage2BGM");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}

	m_Result = m_pSystem->createSound("Sounds/SE/Purify.mp3", FMOD_LOOP_OFF, 0, &pSound);
	pName = _T("Purify");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}


	m_Result = m_pSystem->createSound("Sounds/SE/Item.mp3", FMOD_LOOP_OFF, 0, &pSound);
	pName = _T("Item");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}

	m_Result = m_pSystem->createSound("Sounds/SE/Attack.mp3", FMOD_LOOP_OFF, 0, &pSound);
	pName = _T("Attack");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}


	m_Result = m_pSystem->createSound("Sounds/SE/Attacked.mp3", FMOD_LOOP_OFF, 0, &pSound);
	pName = _T("Attacked");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}
	
	m_Result = m_pSystem->createSound("Sounds/SE/NumberWrong.mp3", FMOD_LOOP_OFF, 0, &pSound);
	pName = _T("WrongNumber");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}

	m_Result = m_pSystem->createSound("Sounds/SE/NumberRight.mp3", FMOD_LOOP_OFF, 0, &pSound);
	pName = _T("RightNumber");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}

	m_Result = m_pSystem->createSound("Sounds/SE/door.mp3", FMOD_LOOP_OFF, 0, &pSound);
	pName = _T("door");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}

	m_Result = m_pSystem->createSound("Sounds/SE/door2.mp3", FMOD_LOOP_OFF, 0, &pSound);
	pName = _T("door2");
	if (m_Result == FMOD_OK)
	{
		m_mapSound.insert(make_pair(pName, pSound));
	}


	m_Result = m_pSystem->update();
	ErrorCheck(m_Result);
}

void CSoundMgr::PlayEffectSound(TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStringCMP(pSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = m_pSystem->playSound(iter->second, 0, false, &m_pEffect);
	ErrorCheck(m_Result);
}

void CSoundMgr::PlayBGMSound(TCHAR* pSoundKey)
{
	StopBGM();
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStringCMP(pSoundKey));

	if(iter == m_mapSound.end())
		return;

	iter->second->setMode(FMOD_LOOP_NORMAL);

	m_Result = m_pSystem->playSound((*iter).second, 0, false, &m_pMainBGM);
	ErrorCheck(m_Result);
}

void CSoundMgr::PlaySkillSound(TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStringCMP(pSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = m_pSystem->playSound((*iter).second, 0, false, &m_pSkill);
	ErrorCheck(m_Result);
}

void CSoundMgr::PlayWaterSound(TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStringCMP(pSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = m_pSystem->playSound((*iter).second, 0, false, &m_pWater);
	ErrorCheck(m_Result);
}

void CSoundMgr::StopBGM(void)
{
	m_Result = m_pMainBGM->stop();
	ErrorCheck(m_Result);
}

void CSoundMgr::StopWater(void)
{
	m_Result = m_pWater->stop();
	ErrorCheck(m_Result);
}

void CSoundMgr::StopALL(void)
{
	m_Result = m_pMainBGM->stop();
	ErrorCheck(m_Result);

	m_Result = m_pEffect->stop();
	ErrorCheck(m_Result);

	m_Result = m_pSkill->stop();
	ErrorCheck(m_Result);

	m_Result = m_pWater->stop();
	ErrorCheck(m_Result);
}


void CSoundMgr::ErrorCheck(FMOD_RESULT _result)
{
	if(_result != FMOD_OK)
	{
		//cout << "¿À·ù : " << _result << endl;
		//system("pause");
		return;
	}
}
