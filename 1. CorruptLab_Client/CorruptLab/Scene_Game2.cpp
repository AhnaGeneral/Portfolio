#include "stdafx.h"
#include "Scene_Game2.h"
#include "Mgr_Collision.h"
#include "Object_Floor.h"
#include "Object_DynamicObj.h"
#include "Shader_Standard.h"
#include "Monster_Boss.h"
#include "Shader_Noise.h"
#include "Shader_BillBoard.h"
#include "Mgr_IndoorControl.h"
#include "CSystem_Particle.h"
#include "Mgr_Item.h"
#include "CNarrationMgr.h"
#include "Shader_Effect.h"

//Scene2-----------------------------------------------------------------------------
CGameScene2::CGameScene2()
{
	m_pPlayer = NULL;
	m_pTerrain = NULL;
	m_pSkyBox = NULL;
	m_pCeiling = NULL;
	m_pFloor = NULL;
	m_pBoss = NULL;

	WindowCursorPos = { 0L,0L };

	itemRange = 0.0f;

	m_bPipelineStateIndex = 0;
	m_ptOldCursorPos = { 0L, 0L };

	m_nDynamicObjectTypeNum = 0;
	m_nStaticObjectTypeNum = 0;
	m_nMonsterTypeNum = 0;

	m_pStaticObjLists = NULL;
	m_pDynamicObjLists = NULL;
	m_pMonsterLists = NULL;
	m_pSoftParticleShader = NULL;

	m_pShadowCamera = NULL;

	m_pShadowMap = NULL;
	m_pDepthTex = NULL;

	m_AnimationTime = 0.0f; 
	m_AnimationControl = true;
	CMgr_IndoorControl::GetInstance()->Initialize();

	//m_pPassWordTexture = NULL; 
	
	//m_pLabatoryPos = new vector<XMFLOAT3>; 
}

CGameScene2::~CGameScene2()
{
	for (auto _autoPaticle : m_pIndoorParticleSystems)
	{
		_autoPaticle->Shutdown();
	}
}

void CGameScene2::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pFloor = new CFloor(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pFloor->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	m_pCeiling = new CFloor(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pCeiling->SetPosition(XMFLOAT3(0.0f, 27.f, 0.0f));
	m_pCeiling->Rotate(180, 0, 0);
	m_pCeiling->UpdateTransform(NULL);


	XMFLOAT3 vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 vRight = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Look = XMFLOAT3(0.0f, -0.8f, 0.01f);
	XMFLOAT3 m_xmf3Right = Vector3::CrossProduct(vUp, m_xmf3Look);
	XMFLOAT3 m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right);

	m_pShadowCamera = new CSunCamera(XMFLOAT3(-20.f, 200.f, 0.f), m_xmf3Look, m_xmf3Right, m_xmf3Up);

	CShader* TexcoordShader = new CTexcoordStandardShader();
	TexcoordShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	TexcoordShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	TexcoordShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1,2); //16

	CGameObject* pBossModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice,
		pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Monster/Boss.bin", NULL, 1);

	m_pBoss = new CBoss();
	m_pBoss->SetChild(pBossModel);
	m_pBoss->SetPosition(XMFLOAT3(0, 7, 0));
	m_pBoss->SetScale(9, 9, 9);
	m_pBoss->Rotate(0, 90, 0);

	//CShader* PlaneLineShader = new CStandardShader();
	//PlaneLineShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	//PlaneLineShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//PlaneLineShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 2); //16

	m_IndoorWall = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice,
		pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Indoor2_(1).bin", TexcoordShader, 0);
	m_IndoorWall->SetPosition(0.f, 0.0f, 0.f);
	m_IndoorWall->SetScale(63.53762f, 21.115f, 77.93047f);

	CTriangleRect* lenghtMesh = new CTriangleRect(pd3dDevice, pd3dCommandList, 1200 / 7.0f,
		800 / 7.f, 0.0f, 1.0f);

	CTriangleRect* widthMesh = new CTriangleRect(pd3dDevice, pd3dCommandList, 0,
		800 / 7.f, 1200 / 7.0f, 1.0f);

	m_IndoorWallLine = new CGameObject * [5];

	Shader_Basic* pShader = new Shader_Basic();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 0); //16

	CGameObject* m_IndoorWallLines = new CGameObject();
	m_IndoorWallLines->SetShader(pShader);
	m_IndoorWallLines->SetMesh(widthMesh);
	m_IndoorWallLines->SetScale(0.f, 0.007f, 0.3f);
	m_IndoorWallLines->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_IndoorWallLines->SetPosition(102, 4.f, +40);
	m_IndoorWallLine[0] = m_IndoorWallLines;

	m_IndoorWallLines = new CGameObject();
	m_IndoorWallLines->SetShader(pShader);
	m_IndoorWallLines->SetMesh(widthMesh);
	m_IndoorWallLines->SetScale(0.f, 0.007f, 0.3f);
	m_IndoorWallLines->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_IndoorWallLines->SetPosition(102, 4.f, -30);
	m_IndoorWallLine[4] = m_IndoorWallLines;

	m_IndoorWallLines = new CGameObject();
	m_IndoorWallLines->SetShader(pShader);
	m_IndoorWallLines->SetMesh(widthMesh);
	m_IndoorWallLines->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_IndoorWallLines->SetScale(0.f, 0.007f, 0.6f);
	m_IndoorWallLines->SetPosition(-102, 4.f, 0);

	m_IndoorWallLine[1] = m_IndoorWallLines;
	m_IndoorWallLines = new CGameObject();
	m_IndoorWallLines->SetShader(pShader);
	m_IndoorWallLines->SetMesh(lenghtMesh);
	m_IndoorWallLines->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_IndoorWallLines->SetScale(1.2f, 0.007f, 0.0f);
	m_IndoorWallLines->SetPosition(0.0f, 4.f, 62.6f);

	m_IndoorWallLine[2] = m_IndoorWallLines;
	m_IndoorWallLines = new CGameObject();
	m_IndoorWallLines->SetShader(pShader);
	m_IndoorWallLines->SetMesh(lenghtMesh);
	m_IndoorWallLines->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_IndoorWallLines->SetScale(1.2f, 0.007f, 0.0f);
	m_IndoorWallLines->SetPosition(0.0f, 4.f, -62.0f);
	m_IndoorWallLine[3] = m_IndoorWallLines;

	m_pTipingEffect = new CShader_Effect;
	m_pTipingEffect->BuildObjects(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, XMFLOAT3(0.0f, 12.0f, 23.0f), false, 0.6f);

	m_pd3dDevice = pd3dDevice; 
	m_pd3dCommandList = pd3dCommandList; 

	// emplace particle 
	PlaceObjectsFromFile(pd3dDevice, m_pd3dGraphicsRootSignature, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CGameScene2::ReleaseObjects()
{
	if (m_pFloor)
	{
		m_pFloor->ReleaseUploadBuffers();
		m_pFloor->Release();
		m_pFloor = nullptr; 

		m_pCeiling->ReleaseUploadBuffers();
		m_pCeiling->Release();
		m_pCeiling = nullptr;

	}
	if (m_IndoorWall)
	{
		m_IndoorWall->ReleaseUploadBuffers();
		m_IndoorWall->Release();
		m_IndoorWall = nullptr;
	}

	//if (m_PlaneLine)
	//{
	//	m_PlaneLine->ReleaseUploadBuffers();
	//	m_PlaneLine->Release();
	//	m_PlaneLine = nullptr;
	//}
	if (m_pStaticObjLists) // 오브젝트 Release
	{
		for (int i = 0; i < m_nStaticObjectTypeNum; i++)
		{
			for (auto Obj : *m_pStaticObjLists[i])
			{
				Obj->Release();
			}
			m_pStaticObjLists[i]->clear();
		}
	}
	delete[] m_pStaticObjLists;
	m_pStaticObjLists = NULL;

	//----------------------------------------------
	if (m_pDynamicObjLists) // 오브젝트 Release
	{
		for (int i = 0; i < m_nDynamicObjectTypeNum; i++)
		{
			for (auto Obj : *m_pDynamicObjLists[i])
			{
				Obj->Release();
			}
			m_pDynamicObjLists[i]->clear();
		}
	}
	delete[] m_pDynamicObjLists;
	m_pDynamicObjLists = NULL;

	//----------------------------------------------

	for (auto _autoPaticle : m_pIndoorParticleSystems)
	{
		_autoPaticle->DisconnectList();
		_autoPaticle->Release();
	}

	if (m_pBoss) m_pBoss->Release();


}

ID3D12RootSignature* CGameScene2::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return nullptr;
}

void CGameScene2::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pShadowCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CGameScene2::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < 5; ++i)
	{
		//m_IndoorWallLine[i]->Rotate(0, 10 + (i) * 500, 0);
		m_IndoorWallLine[i]->UpdateShaderVariable(pd3dCommandList, &(m_IndoorWallLine[i]->m_xmf4x4World));
	}
}

void CGameScene2::ReleaseShaderVariables()
{
	for (auto _autoPaticle : m_pIndoorParticleSystems)
	{
		_autoPaticle->ReleaseShaderVariables();
	}

}

bool CGameScene2::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		WindowCursorPos = m_ptOldCursorPos;

		//ScreenToClient함수 전체화면 기준인 커서의를 해당 클라이언트 윈도우 기준으로 좌표를 변환해주는 함수 이다. 
		ScreenToClient(hWnd, &WindowCursorPos);
		//n_ReactItem
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}

	return true;
}

bool CGameScene2::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	float* m_fShadowPosition = m_pShadowCamera->GetShadowCameraPosition();

	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case 'Q':
		case 'q':
			PassWordCheck();
			DeskOpenCheck();
			CodingCheck();
			break;

			if (CMgr_IndoorControl::GetInstance()->GetPasswordControl())
			{
				case'1':
				case'2':
				case'3':
				case'4':
				case'5':
				case'6':
				case'7':
				case'8': 
				case'9':
				case'0':
					CMgr_IndoorControl::GetInstance()->InsertPassword(int(wParam) - 48);
					break;
				case VK_BACK:
					CMgr_IndoorControl::GetInstance()->EraserPassword();
					break;
				case VK_RETURN:
					if (CMgr_IndoorControl::GetInstance()->GetIsCoded(0) == false)
					{
						bool Is_True = CMgr_IndoorControl::GetInstance()->ConfirmDoorPassword();
						CMgr_IndoorControl::GetInstance()->SetThatIsRightPassword(Is_True);
					}
					else
					{

						XMFLOAT3 playerpos = m_pPlayer->GetPosition();
						float MoniterPos1 = Vector3::Length(Vector3::Subtract(playerpos, XMFLOAT3(0, 0, 15)));
						float MoniterPos2 = Vector3::Length(Vector3::Subtract(playerpos, XMFLOAT3(0, 0, -15)));

						if (MoniterPos2 < 5.5)
						{
							bool Is_True = CMgr_IndoorControl::GetInstance()->ConfirmLeftPassword();
						}
						else if(MoniterPos1 < 5.5)
						{
							bool Is_True = CMgr_IndoorControl::GetInstance()->ConfirmRightPassword();

						}
					}
					break;
			}

			break;
		case 'L':
			if (CMgr_IndoorControl::GetInstance()->GetExecuteHandLightControl())
				CMgr_IndoorControl::GetInstance()->SetExecuteHandLightControl(false); 
			else
			{
				CNarrationMgr::GetInstance()->TurnOnNarration(15);
				CMgr_IndoorControl::GetInstance()->SetExecuteHandLightControl(true);
			}

			break; 
		case VK_SPACE:
			m_pPlayer->SetAttackState();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return true;
}

void CGameScene2::DepthRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//if (m_pPlayer) m_pShadowCamera->Update(m_pPlayer->GetCamera());

	m_pShadowCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pShadowCamera->Update(m_pPlayer->GetCamera());

	if (m_pShadowCamera) m_pShadowCamera->UpdateShaderVariables(pd3dCommandList,ROOT_PARAMETER_SHADOWCAMERA);

	if (m_pStaticObjLists) // 오브젝트 Render
	{
		for (int i = 0; i < m_nStaticObjectTypeNum; i++)
		{
			for (auto& obj : *m_pStaticObjLists[i])
			{
				obj->UpdateTransform(NULL);
				obj->Render(pd3dCommandList, pCamera, 1);
			}
		}
	}

	if (m_ThatIsRightPassword)
	{
		if (m_pBoss) m_pBoss->Render(pd3dCommandList, pCamera, 1);
	}

	if (m_pDynamicObjLists) // 오브젝트 Render
	{
		for (int i = 0; i < m_nDynamicObjectTypeNum; i++)
		{
			for (auto Obj : *m_pDynamicObjLists[i])
			{
				Obj->UpdateTrackNumber(Obj->m_iTrackNumber);
				Obj->UpdateAnimationCache(Obj->m_iTrackNumber);
				Obj->UpdateTransform(NULL);
				Obj->Render(pd3dCommandList, pCamera, 1);
			}
		}
	}

	if (m_pFloor) m_pFloor->Render(pd3dCommandList, pCamera, 1);
	if (m_pPlayer) m_pPlayer->Render(pd3dCommandList, pCamera, 1);

}

void CGameScene2::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	if (m_pPlayer) m_pPlayer->Render(pd3dCommandList, pCamera);
	
	if (m_pFloor) m_pFloor->Render(pd3dCommandList, pCamera);
	

	if (m_pCeiling) m_pCeiling->Render(pd3dCommandList, pCamera, 0);

	if (m_IndoorWall) m_IndoorWall->Render(pd3dCommandList, pCamera); 
	
	if (m_ThatIsRightPassword)
	{
		if (m_pBoss)
		{
			m_pBoss->Update(m_fElapsedTime, NULL, NULL);

			m_pBoss->Animate(m_fElapsedTime, NULL, 0);
			//	if (Obj->m_iTrackNumber == 1) continue;
			m_pBoss->UpdateTransform(NULL);
			m_pBoss->Render(pd3dCommandList, pCamera);
		}
	}

	if (m_pStaticObjLists) // 오브젝트 Render
	{
		for (int i = 0; i < m_nStaticObjectTypeNum; i++)
		{
			for (auto& obj : *m_pStaticObjLists[i])
			{
				obj->UpdateTransform(NULL);
				obj->Render(pd3dCommandList, pCamera, 0);
			}
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		m_IndoorWallLine[i]->UpdateTransform(NULL);
		m_IndoorWallLine[i]->Render(pd3dCommandList, pCamera, 0);
	}

	if (m_pDynamicObjLists) // 오브젝트 Render
	{
		for (int i = 0; i < m_nDynamicObjectTypeNum; i++)
		{
			for (auto Obj : *m_pDynamicObjLists[i])
			{
				Obj->UpdateTrackNumber(Obj->m_iTrackNumber);
				Obj->Update(m_fElapsedTime);
				Obj->UpdateTransform(NULL);
				Obj->Render(pd3dCommandList, pCamera, 0);
			}
		}
	}
	for (auto _autoPaticle : m_pIndoorParticleSystems)
	{
		_autoPaticle->Frame(m_pDevice, m_fElapsedTime, pd3dCommandList, pCamera);
	}



	if (m_pTipingEffect&& (m_HoloGramControl[0]|| m_HoloGramControl[1])) 
	{
		
		m_pTipingEffect->Render(pd3dCommandList, pCamera, false); 
	}
	
	CItemMgr::GetInstance()->Update(m_fElapsedTime);
	CItemMgr::GetInstance()->BillboardUIRender(pd3dCommandList, pCamera);
	CheckCollisions();
}

void CGameScene2::Update(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	if (m_pPlayer)
	{
		m_pPlayer->Animate(fTimeElapsed, NULL);
	}
		
	if (m_HoloGramControl[0])
	{
		m_HoloGramRenderTime[0] += fTimeElapsed; 

		if (m_HoloGramRenderTime[0] > 11.0f)
		{
			m_HoloGramRenderTime[0] = 0.0f;
			m_HoloGramControl[0] = false;
		}
	}

	if (m_HoloGramControl[1])
	{
		m_HoloGramRenderTime[1] += fTimeElapsed;

		if (m_HoloGramRenderTime[1] > 11.0f)
		{
			m_HoloGramRenderTime[1] = 0.0f;
			m_HoloGramControl[1] = false;
		}
	}

	if (m_ThatIsRightPassword)
	{
		XMFLOAT3 playerPos = m_pPlayer->GetPosition();
		if (playerPos.x < 35)
			CNarrationMgr::GetInstance()->TurnOnNarration(9);
	}
}
	



void CGameScene2::CodingCheck()
{
	if (CMgr_IndoorControl::GetInstance()->GetIsCoded(0) == false) return;

	if (CMgr_IndoorControl::GetInstance()->GetPasswordControl())
	{
		CMgr_IndoorControl::GetInstance()->SetpasswordControl(false);
		return;
	}

	XMFLOAT3 playerpos = m_pPlayer->GetPosition();
	float MoniterPos1 = Vector3::Length(Vector3::Subtract(playerpos, XMFLOAT3(0, 0, 15))); 
	float MoniterPos2 = Vector3::Length(Vector3::Subtract(playerpos, XMFLOAT3(0, 0, -15)));
	
	if (MoniterPos1 < 5.5 )
	{
		if (CMgr_IndoorControl::GetInstance()->GetIsCoded(2)) return;
		CMgr_IndoorControl::GetInstance()->SetpasswordControl(true);
		


	}

	if (MoniterPos2 < 5.5)
	{
		if (CMgr_IndoorControl::GetInstance()->GetIsCoded(1)) return;
		CMgr_IndoorControl::GetInstance()->SetpasswordControl(true);
	}
}

int tmp = 0;
void CGameScene2::PassWordCheck()
{
	for (auto pObj : *m_pStaticObjLists[OBJECT_INDOOR_TYPE_PASSWORD])
	{
		XMFLOAT3 ObjPos = pObj->GetPosition();
		XMFLOAT3 PlayerPos = m_pPlayer->GetPosition();

		float Distance = Vector3::Length(Vector3::Subtract(ObjPos, PlayerPos));
		if (Distance < 18.f)
		{
			if (tmp == 0)
			{
				tmp = 1; 
			}
			else
			{
				tmp = 0;
			}
			CMgr_IndoorControl::GetInstance()->SetpasswordControl(tmp);
		}
	}

}

void CGameScene2::DeskOpenCheck()
{
	for (auto pObj : *m_pDynamicObjLists[OBJECT_INDOOR_TYPE_DESKOPEN])
	{
		XMFLOAT3 ObjPos = pObj->GetPosition();
		XMFLOAT3 PlayerPos = m_pPlayer->GetPosition();
		
		float Distance = Vector3::Length(Vector3::Subtract(ObjPos, PlayerPos));
		
		if (Distance < 13.f)
		{
			if (pObj->m_iTrackNumber == 7)
			{
				CMgr_IndoorControl::GetInstance()->SetDeskOpenControl(pObj->m_iTrackNumber);
				CItemMgr::GetInstance()->GetPassword(ObjPos);
			}
		}
	}
}

void CGameScene2::IndoorParticleEffectRender()
{
	for (auto a : m_DrugmakerImpromation)
	{
		ParticleSystemObject* m_pIndoorParticleSystemObject = new ParticleSystemObject(m_pd3dDevice, m_pd3dCommandList,
			m_pd3dGraphicsRootSignature, XMFLOAT3(a.pos.x, a.pos.y + 2, a.pos.z), 4.0f + a.size.y * 3.0f);

		m_pIndoorParticleSystemObject->InitializeParticleSystem(2.5f, 250.0f, 300.f);
		m_pIndoorParticleSystemObject->InitializeBuffer(m_pd3dDevice, m_pd3dCommandList);
		m_pIndoorParticleSystemObject->CreateParticleShaderTexture(m_pd3dDevice, m_pd3dCommandList, m_pd3dGraphicsRootSignature);
		m_pIndoorParticleSystemObject->RegenerateParticles(XMFLOAT3(a.pos.x, a.pos.y + 2, a.pos.z));
		m_pIndoorParticleSystems.push_back(m_pIndoorParticleSystemObject);
	}
	for (int i = 0; i < 5; i++)
	{
		ParticleSystemObject *m_pLastParticlesystemObject = new ParticleSystemObject(m_pd3dDevice, m_pd3dCommandList,
			m_pd3dGraphicsRootSignature, XMFLOAT3(135.0f, 3.0f, 10.0f - (i*5)), 4.0f + 7.0f * 3.0f);

		m_pLastParticlesystemObject->InitializeParticleSystem(2.5f, 250.0f, 300.f);
		m_pLastParticlesystemObject->InitializeBuffer(m_pd3dDevice, m_pd3dCommandList);
		m_pLastParticlesystemObject->CreateParticleShaderTexture(m_pd3dDevice, m_pd3dCommandList, m_pd3dGraphicsRootSignature);
		m_pLastParticlesystemObject->RegenerateParticles(XMFLOAT3(135.0f, 3.0f, 8.0f - (i * 5)));
		m_pIndoorParticleSystems.push_back(m_pLastParticlesystemObject);
	}
}

void CGameScene2::CodeMachine(bool r) // true: right, false : left
{
	m_pPlayer->SetType();
	if (r)
	{
		m_HoloGramControl[0] = true;
		m_HoloGramControl[1] = false;
	}
	else
	{

		m_HoloGramControl[1] = true;
		m_HoloGramControl[0] = false;
	}
	BuildHoloGram();
}

void CGameScene2::BuildHoloGram()
{
	if (m_pTipingEffect)
	{
		if (m_HoloGramControl[0])
			m_pTipingEffect->UpdatePosition(XMFLOAT3(0.0f, 12.0f, 23.0f));

		if(m_HoloGramControl[1])
			m_pTipingEffect->UpdatePosition(XMFLOAT3(0.0f, 12.0f, -23.0f));
	}
}

void CGameScene2::SetThatIsRightPassword(bool _ThatIsRightPassword)
{
	m_ThatIsRightPassword = _ThatIsRightPassword;
}

bool CGameScene2::ProcessInput(UCHAR* pKeysBuffer, HWND hWnd)
{
	DWORD dwDirection = 0;
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;

	if (!CNarrationMgr::GetInstance()->m_bRender)
	{

		if (pKeysBuffer[0x57] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeysBuffer[0x53] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeysBuffer[0x41] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeysBuffer[0x44] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
		if (GetCapture() == hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}

	}


	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeysBuffer[VK_RBUTTON] & 0xF0)
			{
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				m_pShadowCamera->Rotate(cyDelta, 0.0f, -cxDelta);

			}
			else
			{
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
		}
		if (dwDirection)
			m_pPlayer->Move(dwDirection, m_fElapsedTime, true);
	}
	m_pPlayer->Update(m_fElapsedTime);

	return true;
}

void CGameScene2::AnimateObjects(float fTimeElapsed)
{
	float fTime = fTimeElapsed;

	m_AnimationTime += fTimeElapsed;

	if (m_AnimationTime > 2.5f)
	{
		m_AnimationTime = 0.0f;
		
		if (m_AnimationControl)
		{
			m_AnimationControl = false;
		}
		else
		{
			m_AnimationControl = true;
		}
	}

	if (m_AnimationControl)
	{
		fTime = (-(fTime - 0.002f));
	}
	else
	{
		fTime = (+(fTime - 0.002f));
	}

	if (m_pStaticObjLists)
	{
		for (int i = 1 ; i < 7; i++)
		{
			if (m_pStaticObjLists[i])
			{
				for (auto& obj : *m_pStaticObjLists[i])
				{
					XMFLOAT3 TmpPosition = obj->GetPosition();
					obj->SetPosition(TmpPosition.x, TmpPosition.y + fTime, TmpPosition.z);
				}
			}
		}
	}

	if (m_ThatIsRightPassword && m_NarationControl)
	{
		XMFLOAT3 ObjPos = XMFLOAT3(118.0f, 0.0f, 3.0f);
		XMFLOAT3 PlayerPos = m_pPlayer->GetPosition();

		float Distance = Vector3::Length(Vector3::Subtract(ObjPos, PlayerPos));
		if (Distance < 8)
		{
			CNarrationMgr::GetInstance()->TurnOnNarration(5);
			m_NarationControl = false;
		}
	}
}

void CGameScene2::ReleaseUploadBuffers()
{
	if (m_pStaticObjLists) // 오브젝트 Release
	{
		for (int i = 0; i < m_nStaticObjectTypeNum; i++)
		{
			if (m_pStaticObjLists[i])
			{
				for (auto& obj : *m_pStaticObjLists[i])
					obj->ReleaseUploadBuffers();
			}
		}
	}
	//----------------------------------------------
	if (m_pDynamicObjLists) // 오브젝트 Release
	{
		for (int i = 0; i < m_nDynamicObjectTypeNum; i++)
		{
			for (auto Obj : *m_pDynamicObjLists[i])
			{
				Obj->ReleaseUploadBuffers();
			}
		}
	}
}
