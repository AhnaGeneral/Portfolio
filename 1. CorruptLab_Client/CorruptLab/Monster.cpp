#include "Monster.h"
#include "Object_UI.h"
#include "Animation.h"
#include "Object_Terrain.h"

CMonster::CMonster()
{
	m_iMaxHP = 100;
	m_iCurrentHP = 40;
	m_bNotice = false;
	m_fDistanceToPlayer = 0;
	m_fSpeed = 10;
	m_fIdleTick = 0;
	m_xmf3RandomMoveDest = XMFLOAT3(0, 0, 0);
	m_bIsPurified = false;
}

CMonster::~CMonster()
{
	if (m_pd3dDissolveTime)
		m_pd3dDissolveTime->Release();
	if (m_HPUI)
	{
		m_HPUI->ReleaseShaderVariables();
		m_HPUI->Release();
	}
}

void CMonster::Initialize(XMFLOAT3 FiledPos, int iAtt)
{
	m_xmf3FiledCenter = FiledPos;
	m_iAtt = iAtt;

}

float CMonster::MoveToTarget(XMFLOAT3& pos, float fTimeElapsed, float Speed, CHeightMapTerrain* pTerrain)
{
	XMFLOAT3 xmf3Position = GetPosition();
	
	XMFLOAT3 xmf3Look = GetLook();
	XMFLOAT3 xmf3ToTarget = Vector3::Subtract(pos, xmf3Position, true);
	float fDotProduct = Vector3::DotProduct(xmf3Look, xmf3ToTarget);
	float fAngle = ::IsEqual(fDotProduct, 1.0f) ? 0.0f : ((fDotProduct > 0.0f) ? XMConvertToDegrees(acos(fDotProduct)) : 90.0f);
	XMFLOAT3 xmf3CrossProduct = Vector3::CrossProduct(xmf3Look, xmf3ToTarget);
	float RotateYaw = fAngle * fTimeElapsed * ((xmf3CrossProduct.y > 0.0f) ? 1.0f : -1.0f);
	if (isnan(RotateYaw))
		RotateYaw = 0;
	Rotate(0.0f, RotateYaw, 0.0f);

	if (m_fDistanceToPlayer < 12.f && m_iState == MONSTER_STATE_ATTACK) return 0;

	XMFLOAT3 MovePos = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Look, Speed * fTimeElapsed));
	if (pTerrain)
		MovePos.y = pTerrain->GetHeight(MovePos.x, MovePos.z) + m_fHarfHeight;

	SetPosition(MovePos);
	return RotateYaw;
}

void CMonster::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	if (m_iState == MONSTER_STATE_PURIFYING && nPipelineState == 0)
		nPipelineState = 2;
	CGameObject::Render(pd3dCommandList, pCamera, nPipelineState);
	if (m_HPUI && nPipelineState == 0)
	{
		m_HPUI->UpdateTransform(&m_xmf4x4World);
		m_HPUI->Render(pd3dCommandList, pCamera);

	}
}

void CMonster::SetHPUI(CUI_MonsterHP* pHP)
{
	m_HPUI = pHP;
	m_HPUI->m_pcbMonsterHP = &m_iCurrentHP;

}

void CMonster::Update(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent, void* pContext)
{
	if (m_bIsPurified)
		GoodUpdate(fTimeElapsed, pxmf4x4Parent, pContext);
	else
	{
		if (m_iState != MONSTER_STATE_PURIFYING)
			BadUpdate(fTimeElapsed, pxmf4x4Parent, pContext);
		else
			PurifyingUpdate(fTimeElapsed, pxmf4x4Parent, pContext);
	}

}

void CMonster::BadUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent, void* pContext)
{
	
}

void CMonster::GoodUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent, void* pContext)
{
}

void CMonster::PurifyingUpdate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent, void* pContext)
{
	if (m_fFurifyingTime < 4)
		m_fDissolveTime += fTimeElapsed / 4;
	else
		m_fDissolveTime -= fTimeElapsed / 4;

	m_fFurifyingTime += fTimeElapsed;
	if (m_fFurifyingTime > 8.f)
	{
		m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_bEnable = true;
		m_iState = MONSTER_STATE_IDLE;
		m_iCurrentHP = 100;
		m_bIsPurified = true;
	}
}

void CMonster::GetPurified()
{
	m_iState = MONSTER_STATE_PURIFYING;
}


void CMonster::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);

	UINT ncbElementBytes = ((sizeof(CB_NOISEBUFFERTYPE) + 255) & ~255);
	m_pd3dDissolveTime = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dDissolveTime->Map(0, NULL, (void**)&m_pcbMappedNoiseBuffers);
}

void CMonster::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	CGameObject::UpdateShaderVariable(pd3dCommandList, pxmf4x4World);
	
	if (m_iState == MONSTER_STATE_PURIFYING)
	{
		::memcpy(&m_pcbMappedNoiseBuffers->frameTime, &m_fDissolveTime, sizeof(float));
		D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dDissolveTime->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_NOISEBUFFER, d3dGpuVirtualAddress);

	}
	
}

void CMonster::OnInitialize()
{
	//m_pAnimationController = new CAnimationController();
	//*m_pAnimationController = *(m_pChild->m_pAnimationController);
}

void CMonster::GetDamaage(int iDamage)
{
	if (m_bIsPurified) return;
	m_iState = MONSTER_STATE_DAMAGEING;
	m_iCurrentHP -= iDamage;
	m_pChild->m_pAnimationController->m_pAnimationTracks[m_iTrackNumber].m_fPosition = 0;
	if (m_iCurrentHP <= 20) 
	{
		if (m_iState == MONSTER_STATE_RETURNING)
			m_bNotice = true;
		SetAnimationSet(m_iStunAnimation, m_iTrackNumber);
 		m_iState = MONSTER_STATE_STUN;
		m_iCurrentHP = 20;
	}
}
