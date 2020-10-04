//------------------------------------------------------- ----------------------
// File: Animation.h
//-----------------------------------------------------------------------------

#pragma once

#define ANIMATION_TYPE_ONCE			0
#define ANIMATION_TYPE_LOOP			1
#define ANIMATION_TYPE_PINGPONG		2
#define ANIMATION_CALLBACK_EPSILON	0.015f

class CGameObject; 
class CSkinnedMesh; 
class CMesh;
class CStandardMesh;

struct CALLBACKKEY
{
	float  				          m_fTime = 0.0f;
	void                        * m_pCallbackData = NULL;
};

//#define _WITH_ANIMATION_SRT
#define _WITH_ANIMATION_INTERPOLATION

class CAnimationSet
{
public:
	CAnimationSet();
	~CAnimationSet();

public:
	char							m_pstrName[64];

	float							m_fLength = 0.0f;
	int								m_nFramesPerSecond = 0; //m_fTicksPerSecond

	int								m_nAnimationBoneFrames = 0;

	int								m_nKeyFrameTransforms = 0;
	float                         * m_pfKeyFrameTransformTimes = NULL;
	XMFLOAT4X4                   ** m_ppxmf4x4KeyFrameTransforms = NULL;

#ifdef _WITH_ANIMATION_SRT
	int								m_nKeyFrameScales = 0;
	float* m_pfKeyFrameScaleTimes = NULL;
	XMFLOAT3** m_ppxmf3KeyFrameScales = NULL;
	int								m_nKeyFrameRotations = 0;
	float* m_pfKeyFrameRotationTimes = NULL;
	XMFLOAT4** m_ppxmf4KeyFrameRotations = NULL;
	int								m_nKeyFrameTranslations = 0;
	float* m_pfKeyFrameTranslationTimes = NULL;
	XMFLOAT3** m_ppxmf3KeyFrameTranslations = NULL;
#endif

	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	int 							m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong

	int								m_nCurrentKey = -1;

	int 							m_nCallbackKeys = 0;
	CALLBACKKEY                    * m_pCallbackKeys = NULL;

public:
	float GetPosition(float fPosition);
	XMFLOAT4X4 GetSRT(int nFrame, float fPosition);
	XMFLOAT4X4 GetSRTSimple(int nFrame, int nPosition);

	void SetCallbackKeys(int nCallbackKeys);
	void SetCallbackKey(int nKeyIndex, float fTime, void* pData);

	void* GetCallback(float fPosition) { return(NULL); }
};

class CAnimationTrack
{
public:
	CAnimationTrack() {}
	void CreateBuffers(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~CAnimationTrack();

public:
	BOOL 							m_bEnable = true;
	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	float 							m_fWeight = 1.0f;

	ID3D12Resource*					m_ppd3dcbSkinningBoneTransforms = NULL; //[SkinnedMeshes]
	XMFLOAT4X4*						m_ppcbxmf4x4MappedSkinningBoneTransforms = NULL;

	CAnimationSet                  * m_pAnimationSet = NULL;
};

class CAnimationCallbackHandler
{
public:
	virtual void HandleCallback(void* pCallbackData) { }
};

class CAnimationController
{
public:
	CAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* LoadGameobject , int nAnimationTracks = 1);
	~CAnimationController();

public:
	float 							m_fTime = 0.0f;

	int								m_nAnimationSets = 0;
	CAnimationSet                 * m_pAnimationSets = NULL;

	int								m_nAnimationSet = 0;

	int								m_nAnimationBoneFrames = 0;
	CGameObject                  ** m_ppAnimationBoneFrameCaches = NULL;
	int 							m_nAnimationTracks = 0;
	CAnimationTrack               * m_pAnimationTracks = NULL;

	CMesh                        * m_pMesh = NULL;
	CSkinnedMesh                 * m_ppSkinnedMeshes = NULL;

	int  				 			m_nAnimationTrack = 0;

public:
	void UpdateTransformCache(int iTrackNum);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int iNum);
	void SetAnimationSet(int nAnimationSet, int iNum = 0);

	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void* pData);

	void AdvanceTime(float fElapsedTime, CAnimationCallbackHandler* pCallbackHandler, int iTrackNum = 0);
};