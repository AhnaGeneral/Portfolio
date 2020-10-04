#include "EndingScene.h"
#include "Monster_Boss.h"

CEndingScene::CEndingScene(CPlayer* player, CBoss* boss)
{
	m_pPlayer = player;
	m_pBoss = boss;
}

ID3D12RootSignature* CEndingScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return nullptr;
}

void CEndingScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pAnimals->SetScale(1.f, 1.f, 1.f);
	m_pAnimals->SetPosition(-25, -3,2);
	m_pAnimals->Rotate(0, -120, 0);
	m_pAnimals->UpdateTransform(NULL);
	m_pBoss->SetPosition(-10.f, -4.f, 0);
	m_pBoss->Rotate(0, 90, 0);
	m_pBoss->SetAnimationSet(1, 0);
}

bool CEndingScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CEndingScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CEndingScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);


	if (m_pBoss)
	{
		m_pBoss->Update(m_fElapsedTime, NULL, NULL);
		m_pBoss->Animate(m_fElapsedTime, NULL, 0);
		//	if (Obj->m_iTrackNumber == 1) continue;
		m_pBoss->UpdateTransform(NULL);
		m_pBoss->Render(pd3dCommandList, pCamera);
	}
	if (m_pAnimals)
		m_pAnimals->Render(pd3dCommandList, pCamera);

}

void CEndingScene::Update(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;
}

bool CEndingScene::ProcessInput(UCHAR* pKeysBuffer, HWND hWnd)
{
	return false;
}
