#pragma once

#include "Scene.h"


class CBoss;
class CFloor;
class Shader_Basic; 
class Shader_TextureBasic;
class ParticleSystemObject;

class DrugMakersImpormation
{
public:
	XMFLOAT3 pos;
	XMFLOAT3 size;
	
public:
	DrugMakersImpormation(XMFLOAT3 _pos, XMFLOAT3 _size) :pos(_pos), size(_size) {}
};

class CGameScene2 : public CGameScene
{
public:
	POINT WindowCursorPos;

	CBoss* m_pBoss;
	CPlayer* m_pPlayer;
	CSkyBox* m_pSkyBox;
	CObjectWater* m_pCObjectWater;
	CFloor * m_pFloor;
	CFloor* m_pCeiling;

	CShader_Effect* m_pGreenZoonEffect;
	CShader_Effect* m_pTipingEffect;

	CGameObject** m_IndoorWallLine = nullptr;
	CGameObject* m_IndoorWall = nullptr;

	CGameObject* m_TestTexure = nullptr;

	bool m_ThatIsRightPassword = false; 
	bool m_AnimationControl;
	bool m_NarationControl = true;

	bool                        m_HoloGramControl[2]{ false, false };
	float                       m_HoloGramRenderTime[2]{ 0.0f, 0.0f };

	list<ParticleSystemObject*> m_pIndoorParticleSystems;
	list<DrugMakersImpormation> m_DrugmakerImpromation;

	int  n_ReactItem;
	float itemRange;
	float m_AnimationTime;


public:
	CGameScene2();
	virtual ~CGameScene2();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects();


	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void DepthRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	virtual void Update(float fTimeElapsed);

	void CodeMachine(bool r);
	void BuildHoloGram();
	void CodingCheck();
	void PassWordCheck(); 
	void DeskOpenCheck(); 
	void IndoorParticleEffectRender();
	void SetThatIsRightPassword(bool _ThatIsRightPassword);
	virtual bool ProcessInput(UCHAR* pKeysBuffer, HWND hWnd);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseUploadBuffers();
	CGameObject* GetAnimals()
	{
		return m_pStaticObjLists[OBJECT_INDOOR_TYPE_ANIMALALL]->front();
	}

	void PlaceObjectsFromFile(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList);
	void PlaceStaticObjectsFromFile(CGameObject* pModel, char* FileName, UINT index);
	void PlaceDynamicFromFile(CGameObject* pModel, char* FileName, int index);
	void PlaceMonsterFromFile(CGameObject* pModel, char* FileName, int index, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ExcuteAnimationDoor();
	void ExcuteAnimationDesk(int _TrackNumber);

private:
	bool					    m_bPipelineStateIndex;
	POINT					    m_ptOldCursorPos;

	int						    m_nDynamicObjectTypeNum;
	int						    m_nStaticObjectTypeNum; // 오브젝트 종류 개수
	int						    m_nMonsterTypeNum;      // 몬스터 종류 개수

	CDynamicObject            * m_pIndoorDrugMaker = nullptr;
	vector<CGameObject*>     ** m_pStaticObjLists;
	vector<CDynamicObject*>  ** m_pDynamicObjLists;
	vector<CMonster*>        ** m_pMonsterLists;
	CSoftParticleShader       * m_pSoftParticleShader;
	CShader_SpecialFog        * m_pSpecialFogShader;

public:

	CCamera                   * m_pShadowCamera;
	CTexture                  * m_pShadowMap;
	CTexture                  * m_pDepthTex;
};
