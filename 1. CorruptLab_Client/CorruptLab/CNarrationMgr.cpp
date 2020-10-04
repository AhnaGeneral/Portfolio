#include "CNarrationMgr.h"
#include "Shader_BaseUI.h"
#include <string.h>


CNarrationMgr* CNarrationMgr::m_pInstance = NULL;

void CNarrationMgr::TurnOnNarration(int iNum)
{
	if (m_pNarrations.find(iNum)->second->IsEnabled)
	{
		m_iCurrentNum = iNum;
		m_bRender = true;
		m_pNarrations.find(iNum)->second->IsEnabled = false;
	}

}
void CNarrationMgr::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignatureCPlayer)
{


	m_pShader = new CShader_BaseUI();
	m_pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignatureCPlayer, FINAL_MRT_COUNT);
	m_pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1,15);
	CNarration* pNarration;
	
	for (int i = 1; i <= 15; i++)
	{

		pNarration = new CNarration();

		pNarration->SetShader(m_pShader, 1);

		CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
		char filename[45] = "UserInterface/Narrations/Narration_1.dds";
		if (i > 9)
		{
			filename[35] = 49;
			filename[36] = i % 10 + 48;
			filename[37] = '.';
			filename[38] = 'd';
			filename[39] = 'd';
			filename[40] = 's';
		}
		else
		filename[35] = i+48;
		wchar_t wc[45];
		for (int i = 0; i < 45; i++)
			wc[i] = filename[i];

		pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, wc, 0);
		m_pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, ROOT_PARAMETER_HP_TEX, 0);
		pNarration->SetTexture(pTexture);
		CMesh* mesh = new CTriangleRect(pd3dDevice, pd3dCommandList, FRAME_BUFFER_WIDTH, (FRAME_BUFFER_HEIGHT /1.7f), 0.0f, 1.0f);
		pNarration->Set2DPosition(0, -(FRAME_BUFFER_HEIGHT /5.4f));
		pNarration->SetMesh(mesh);
		pNarration->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		float* alpha = new float(1);
		pNarration->SetAlpha(alpha);
		m_pNarrations.insert(make_pair(i, pNarration));

	}

}

void CNarrationMgr::Update(float fElapsedTime, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_bRender)
	{
		m_pShader->Render(pd3dCommandList, pCamera, 0);
		m_pNarrations.find(m_iCurrentNum)->second->Render(pd3dCommandList, pCamera, 0);
		m_fLifetime += fElapsedTime;
		if (m_fLifetime > 8.f)
		{
			m_bRender = false;
			m_fLifetime = 0.0f;
			if (m_iCurrentNum == 5) TurnOnNarration(6);
		}
	}
}

void CNarrationMgr::Skip()
{
	if (m_bRender)
	{
		m_bRender = false;
		m_fLifetime = 0.0f;
		switch (m_iCurrentNum)
		{
		case 5: TurnOnNarration(6);
			break;
		case 9: TurnOnNarration(10);
			break;
		case 10: TurnOnNarration(8);
			break;
		case 8: TurnOnNarration(11);
			break;
		}
	}
}

void CNarrationMgr::Destroy()
{
	for (int i = 0; i < m_pNarrations.size(); i++)
	{
		m_pNarrations.find(i+1)->second->Release();
	}
	//if (m_pShader) m_pShader->Release();

	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}

}
