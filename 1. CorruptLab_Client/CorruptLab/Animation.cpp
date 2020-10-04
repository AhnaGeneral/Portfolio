//-----------------------------------------------------------------------------
// File: CAnimaition.cpp
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "Animation.h"
//#include "Shader.h"
#include "Shader_Standard.h"
#include "Scene.h"
#include "Object.h"
#include "Mesh.h"

CAnimationSet::CAnimationSet()
{
}

CAnimationSet::~CAnimationSet()
{
	if (m_pfKeyFrameTransformTimes)
		delete[] m_pfKeyFrameTransformTimes;

	for (int j = 0; j < m_nKeyFrameTransforms; j++)
	{
		if (m_ppxmf4x4KeyFrameTransforms[j])
			delete[] m_ppxmf4x4KeyFrameTransforms[j];
	}
	if (m_ppxmf4x4KeyFrameTransforms) 
		delete[] m_ppxmf4x4KeyFrameTransforms;

	if (m_pCallbackKeys) 
		delete[] m_pCallbackKeys;

}

float CAnimationSet::GetPosition(float fPosition)
{
	float fGetPosition = fPosition;
	switch (m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
		fGetPosition = fPosition - int(fPosition / m_pfKeyFrameTransformTimes[m_nKeyFrameTransforms - 1]) * m_pfKeyFrameTransformTimes[m_nKeyFrameTransforms - 1];
				//	fGetPosition = fPosition - int(fPosition / m_fLength) * m_fLength;
#ifdef _WITH_ANIMATION_INTERPOLATION			
#else
		m_nCurrentKey++;
		if (m_nCurrentKey >= m_nKeyFrameTransforms) m_nCurrentKey = 0;
#endif
		break;
	}
	case ANIMATION_TYPE_ONCE:
	{
		//fGetPosition = fPosition - int(fPosition / m_pfKeyFrameTransformTimes[m_nKeyFrameTransforms - 1]) * m_pfKeyFrameTransformTimes[m_nKeyFrameTransforms - 1];		
		fGetPosition = fPosition;
		break;
	}
	case ANIMATION_TYPE_PINGPONG:
		break;
	}
	return(fGetPosition);
}

XMFLOAT4X4 CAnimationSet::GetSRT(int nFrame, float fPosition)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Identity();
#ifdef _WITH_ANIMATION_INTERPOLATION
#ifdef _WITH_ANIMATION_SRT
	XMVECTOR S, R, T;
	for (int i = 0; i < (m_nKeyFrameTranslations - 1); i++)
	{
		if ((m_pfKeyFrameTranslationTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameTranslationTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameTranslationTimes[i]) / (m_pfKeyFrameTranslationTimes[i + 1] - m_pfKeyFrameTranslationTimes[i]);
			T = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i][nFrame]), XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i + 1][nFrame]), t);
			break;
		}
	}
	for (UINT i = 0; i < (m_nKeyFrameScales - 1); i++)
	{
		if ((m_pfKeyFrameScaleTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameScaleTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameScaleTimes[i]) / (m_pfKeyFrameScaleTimes[i + 1] - m_pfKeyFrameScaleTimes[i]);
			S = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameScales[i][nFrame]), XMLoadFloat3(&m_ppxmf3KeyFrameScales[i + 1][nFrame]), t);
			break;
		}
	}
	for (UINT i = 0; i < (m_nKeyFrameRotations - 1); i++)
	{
		if ((m_pfKeyFrameRotationTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameRotationTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameRotationTimes[i]) / (m_pfKeyFrameRotationTimes[i + 1] - m_pfKeyFrameRotationTimes[i]);
			R = XMQuaternionSlerp(XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i][nFrame]), XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i + 1][nFrame]), t);
			break;
		}
	}

	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixAffineTransformation(S, NULL, R, T));
#else   
	for (int i = 0; i < (m_nKeyFrameTransforms - 1); i++)
	{
		if ((m_pfKeyFrameTransformTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameTransformTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameTransformTimes[i]) / (m_pfKeyFrameTransformTimes[i + 1] - m_pfKeyFrameTransformTimes[i]);
			XMVECTOR S0, R0, T0, S1, R1, T1;
			XMMatrixDecompose(&S0, &R0, &T0, XMLoadFloat4x4(&m_ppxmf4x4KeyFrameTransforms[i][nFrame]));
			XMMatrixDecompose(&S1, &R1, &T1, XMLoadFloat4x4(&m_ppxmf4x4KeyFrameTransforms[i + 1][nFrame]));
			XMVECTOR S = XMVectorLerp(S0, S1, t);
			XMVECTOR T = XMVectorLerp(T0, T1, t);
			XMVECTOR R = XMQuaternionSlerp(R0, R1, t);
			XMStoreFloat4x4(&xmf4x4Transform, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
			break;
		}
	}
#endif
#else
	xmf4x4Transform = m_ppxmf4x4KeyFrameTransforms[m_nCurrentKey][nFrame];
#endif
	return(xmf4x4Transform);
}

XMFLOAT4X4 CAnimationSet::GetSRTSimple(int nFrame, int nPosition)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Identity();
	XMVECTOR S0, R0, T0;
	XMMatrixDecompose(&S0, &R0, &T0, XMLoadFloat4x4(&m_ppxmf4x4KeyFrameTransforms[nPosition][nFrame]));
	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixAffineTransformation(S0, XMVectorZero(), R0, T0));
	return(xmf4x4Transform);
}

void CAnimationSet::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationSet::SetCallbackKey(int nKeyIndex, float fKeyTime, void* pData)
{
	m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationController::CAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,  CGameObject* LoadGameobject, int nAnimationTracks)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks = new CAnimationTrack[nAnimationTracks];
	for (int i = 0; i < m_nAnimationTracks; i++)
		m_pAnimationTracks[i].CreateBuffers(pd3dDevice, pd3dCommandList);
	//m_ppd3dcbSkinningBoneTransforms = new ID3D12Resource;
	//m_ppcbxmf4x4MappedSkinningBoneTransforms = new XMFLOAT4X4 * [m_nSkinnedMeshes];
	m_ppSkinnedMeshes = (CSkinnedMesh*)(LoadGameobject->GetMesh());
	//m_pMesh = (LoadGameobject->GetMesh());

}

CAnimationController::~CAnimationController()
{
	if (m_pAnimationSets) delete[] m_pAnimationSets;
	if (m_ppAnimationBoneFrameCaches) delete[] m_ppAnimationBoneFrameCaches;
	if (m_pAnimationTracks) delete[] m_pAnimationTracks;
}

void CAnimationController::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	m_pAnimationSets[nAnimationSet].m_nCallbackKeys = nCallbackKeys;
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationController::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void* pData)
{
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void CAnimationController::UpdateTransformCache(int iTrackNum)
{
	CAnimationSet* pAnimationSet = m_pAnimationTracks[iTrackNum].m_pAnimationSet;

	if (!pAnimationSet) return;
	float fPositon = pAnimationSet->GetPosition(m_pAnimationTracks[iTrackNum].m_fPosition);
	if (pAnimationSet->m_nType == ANIMATION_TYPE_ONCE && fPositon >= pAnimationSet->m_fLength - 0.1f)
	{
		for (int j = 0; j < m_nAnimationBoneFrames; j++)
		{
			m_ppAnimationBoneFrameCaches[j]->m_xmf4x4Transform = pAnimationSet->GetSRTSimple(j, pAnimationSet->m_nKeyFrameTransforms - 1);
		}
	}
	else
	{
		for (int j = 0; j < m_nAnimationBoneFrames; j++)
		{
			m_ppAnimationBoneFrameCaches[j]->m_xmf4x4Transform = pAnimationSet->GetSRT(j, fPositon);
		}

	}
}

void CAnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int iNum)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneTransformsGpuVirtualAddress = m_pAnimationTracks[iNum].m_ppd3dcbSkinningBoneTransforms->GetGPUVirtualAddress();
	
	for (int i = 0; i < m_ppSkinnedMeshes->m_nSkinningBones; i++)
	{
		XMMATRIX transpose = XMMatrixTranspose(XMLoadFloat4x4(&m_ppSkinnedMeshes->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World));
		XMStoreFloat4x4(&m_pAnimationTracks[iNum].m_ppcbxmf4x4MappedSkinningBoneTransforms[i],transpose);
	}
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_BONE_TRANSFORMS, d3dcbBoneTransformsGpuVirtualAddress);
}

void CAnimationController::SetAnimationSet(int nAnimationSet, int iNum)
{
	if (m_pAnimationSets && (nAnimationSet < m_nAnimationSets))
	{
		m_nAnimationSet = nAnimationSet;
		m_pAnimationTracks[iNum].m_pAnimationSet = &m_pAnimationSets[m_nAnimationSet];
	}
}

void CAnimationController::AdvanceTime(float fTimeElapsed, CAnimationCallbackHandler* pCallbackHandler, int iTrackNum )
{
	m_fTime += fTimeElapsed;

	if (m_pAnimationSets)
	{
		CAnimationSet* pAnimationSet = m_pAnimationTracks[iTrackNum].m_pAnimationSet;
		if (m_pAnimationTracks[iTrackNum].m_bEnable)
		{
			m_pAnimationTracks[iTrackNum].m_fPosition += (fTimeElapsed * pAnimationSet->m_fSpeed);
			 //(fTimeElapsed * pAnimationSet->m_fSpeed);
		}
		pAnimationSet->m_fPosition = m_pAnimationTracks[iTrackNum].m_fPosition;
			if (pCallbackHandler)
			{
				void* pCallbackData = pAnimationSet->GetCallback(pAnimationSet->m_fPosition);
				if (pCallbackData) pCallbackHandler->HandleCallback(pCallbackData);
			}

			float fPositon = pAnimationSet->GetPosition(pAnimationSet->m_fPosition);

			if (pAnimationSet->m_nType == ANIMATION_TYPE_ONCE && fPositon >= pAnimationSet->m_fLength-0.1f)
			{
				for (int j = 0; j < m_nAnimationBoneFrames; j++)
				{
					m_ppAnimationBoneFrameCaches[j]->m_xmf4x4Transform = pAnimationSet->GetSRTSimple(j, pAnimationSet->m_nKeyFrameTransforms - 1);
				}
			}
			else
			{
				for (int j = 0; j < m_nAnimationBoneFrames; j++)
				{
					m_ppAnimationBoneFrameCaches[j]->m_xmf4x4Transform = pAnimationSet->GetSRT(j, fPositon);
				}

			}
		
	}
}

void CAnimationTrack::CreateBuffers(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES)) ); //256ÀÇ ¹è¼ö
	m_ppd3dcbSkinningBoneTransforms = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_ppd3dcbSkinningBoneTransforms->Map(0, NULL, (void**)&m_ppcbxmf4x4MappedSkinningBoneTransforms);
}

CAnimationTrack::~CAnimationTrack()
{
	
}
