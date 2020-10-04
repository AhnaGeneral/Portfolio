#pragma once
#include "Scene.h"

class CBoss;
class CEndingScene : public CScene
{
public:
	CEndingScene(CPlayer* player, CBoss* boss);
	~CEndingScene() {}

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void ReleaseObjects() {}



	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void ReleaseShaderVariables() {}

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void DepthRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL) {}
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	virtual void Update(float fTimeElapsed);


	virtual bool ProcessInput(UCHAR* pKeysBuffer, HWND hWnd);
	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void ReleaseUploadBuffers() {}


public:
	CPlayer* m_pPlayer;
	CBoss* m_pBoss;
	CGameObject* m_pAnimals;
};

