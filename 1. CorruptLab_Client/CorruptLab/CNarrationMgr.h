#pragma once
#include "stdafx.h"
#include "Material.h"
#include "Object_UI.h"
#include "Object_Camera.h"
#include "Shader_BaseUI.h"
#include <map>

using namespace std;

class CNarration : public CUI_Root
{
public:
	bool IsEnabled = true;
	CTexture* m_pTexture = NULL;

};

class CNarrationMgr
{
private:
	int m_iCurrentNum = 1;
	static CNarrationMgr* m_pInstance;
	map<int, CNarration*>  m_pNarrations;

	float m_fLifetime = 0.0f;
	CShader_BaseUI* m_pShader = NULL;

public:
	bool m_bRender = false;
	void TurnOnNarration(int iNum);
	void Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignatureCPlayer);
	void Update(float fElapsedTime, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void Skip();

	static CNarrationMgr* GetInstance(void)
	{
		if (m_pInstance == NULL)
			m_pInstance = new CNarrationMgr();
		return m_pInstance;
	}
	void Destroy();
};

