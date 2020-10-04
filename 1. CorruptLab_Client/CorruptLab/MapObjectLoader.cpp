
#include "stdafx.h"
#include "Scene.h"
#include "Object_StaticObj.h"
#include "Monster_Yangmal.h"
#include "Monster_Tosm.h"
#include "Object_ItemBox.h"
#include "Object_Researcher.h"
#include "Object_DrugMaker.h"
#include "Mgr_Collision.h"
#include "Object_UI.h"
#include "Shader_ObjHP.h"
#include "Shader_BillBoard.h"
#include "Shader_Standard.h"
//#include "Monster_Yangmal.h"

#define MAXSTATICEVEC 100 
#define MAXDYNAMICVEC 100
#define MAXMONSTERVEC 100 

void CGameScene::PlaceObjectsFromFile(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nStaticObjectTypeNum = 9;
	m_pStaticObjLists = new vector<CGameObject*>*[m_nStaticObjectTypeNum];

	for (int i = 0; i < m_nStaticObjectTypeNum; i++)
		m_pStaticObjLists[i] = new vector<CGameObject*> ();

	//------------------------------------------------------------
	m_nDynamicObjectTypeNum = 3;
	m_pDynamicObjLists = new vector<CDynamicObject*> * [m_nDynamicObjectTypeNum];

	for (int i = 0; i < m_nDynamicObjectTypeNum; i++)
		m_pDynamicObjLists[i] = new vector<CDynamicObject*> ();

	//------------------------------------------------------------
	m_nMonsterTypeNum = 2;
	m_pMonsterLists = new vector<CMonster*> * [m_nMonsterTypeNum];

	

	for (int i = 0; i < m_nMonsterTypeNum; i++) // ÃÊ±âÈ­
		m_pMonsterLists[i] = new vector<CMonster*>();
	
	int CurrentIndex = 0;
	CGameObject* pDiverObject = NULL;



	CShader* pAlphaChannelAnimation = new  AlphaChannelAnimationShader();
	pAlphaChannelAnimation->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	pAlphaChannelAnimation->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pAlphaChannelAnimation->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 7); //16



	/*Static*/
	//Rocks-------------------------------------------------------
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Rock.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/Rocks.bin", OBJECT_TYPE_ROCK);

	//Fence-----------------------------------------------
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Fence1.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/Fences.bin", OBJECT_TYPE_FENCE);


	//Tower-----------------------------------------------
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/TRMTower.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/Towers.bin", OBJECT_TYPE_TRMTOWER);

	//AnimalBone------------------------------------------
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/AnimalBon.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/AnimalBones.bin", OBJECT_TYPE_BONE);


	//House-----------------------------------------------
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/HouseAll_1.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/Houses.bin", OBJECT_TYPE_HOUSE);
	pDiverObject->m_nReferences--;
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/HouseAll_2.bin", NULL, 0);
	m_pStaticObjLists[OBJECT_TYPE_HOUSE]->at(1)->m_pChild = NULL;
	m_pStaticObjLists[OBJECT_TYPE_HOUSE]->at(1)->SetChild(pDiverObject);


	//Tree-----------------------------------------------
	CShader* TexcoordShader = new CTreeTexcoordStandardShader();
	TexcoordShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	TexcoordShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	TexcoordShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 2); //16

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Tree1.bin", TexcoordShader, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/Trees.bin", OBJECT_TYPE_TREE);

	//Bridges------------------------------------------
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Bridge.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/Bridges.bin", OBJECT_TYPE_BRIDGE);

	//OpeningDoor------------------------------------------
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Fence2.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/Fence2T.bin", OBJECT_TYPE_FENCE2);

	m_pOpeningWarningDoors = new CGameObject * [2];
	PlaceDynamicFromFile(pDiverObject, "ObjectsData/OpeningDoorsT.bin", OBJECT_TYPE_FENCE2, 1);
	
	
	////Bridges------------------------------------------
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/warningBoard.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/warningT.bin", OBJECT_TYPE_WARNING);



	/*Dynamic*///////////////////////////////////////////
	//ItemBox--------------------------------------------
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Box_Opening.bin", NULL, 2);
	PlaceDynamicFromFile(pDiverObject, "ObjectsData/ItemBoxes.bin", OBJECT_TYPE_ITEMBOX);


	////DrugMakers--------------------------------------------
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/DrugMaker.bin", NULL, 0);
	CGameObject* Alpha = pDiverObject->FindFrame("cylinderAlpha");
	CShader* alphaShader = new CTransparentedStandardShader();
	alphaShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	Alpha->SetShader(0, alphaShader);
	PlaceDynamicFromFile(pDiverObject, "ObjectsData/DrugMakers.bin", OBJECT_TYPE_DRUGMAKER);

	//Researcher------------------------------------------------------
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Researcher.bin", NULL, 2);
	PlaceDynamicFromFile(pDiverObject, "ObjectsData/Researchers.bin", OBJECT_TYPE_RESEARCHER);

	/*Monster*/
	//Yangmal-------------------------------------------------

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Monster/Yangmal.bin", pAlphaChannelAnimation, 4);
	//pDiverObject->SetAnimatationSpeed(3, 0.5f);
	PlaceMonsterFromFile(pDiverObject, "ObjectsData/Yangmals.bin", MONSTER_TYPE_YANGMAL, pd3dDevice,pd3dCommandList);

	//Tosm-------------------------------------------------
	CTexture* dissolveTex = new CTexture(1, RESOURCE_TEXTURE2D, 0);

	dissolveTex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/dissolve.dds", 0);
	pAlphaChannelAnimation->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, dissolveTex, ROOT_PARAMETER_DISSOLVE_MAP, 0);
	dynamic_cast<CSkinnedAnimationShader*>(pAlphaChannelAnimation)->SetDissolveTexture(dissolveTex);


	m_pPurifiedYMBC = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	m_pPurifiedYMBC->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Model/Textures/YangmalFurified_BC.dds"), 0);

	m_pPurifiedYMNM = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	m_pPurifiedYMNM->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Model/Textures/YangmalFurified_NM.dds"), 0);

	pAlphaChannelAnimation->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pPurifiedYMBC, ROOT_PARAMETER_ALBEDO_TEX, 0);
	pAlphaChannelAnimation->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, m_pPurifiedYMNM, ROOT_PARAMETER_NORMAL_TEX, 1);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Monster/Tosm.bin", pAlphaChannelAnimation, 4);

	PlaceMonsterFromFile(pDiverObject, "ObjectsData/Tosms.bin", MONSTER_TYPE_TOSM, pd3dDevice, pd3dCommandList);

	CCollisionMgr::GetInstance()->m_pMonsterLists = m_pMonsterLists;

}



void CGameScene::PlaceStaticObjectsFromFile(CGameObject* pModel, char* FileName, UINT index)
{

	FILE* pInFile = NULL;
	::fopen_s(&pInFile, FileName, "rb");
	::rewind(pInFile);

	UINT nObjects = 0;

	(UINT)::fread_s(&nObjects,sizeof(UINT), sizeof(UINT), 1, pInFile);

	XMFLOAT3 xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f),
		xmf3Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f), 
		xmf3Scale = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4 xmf4Rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	CGameObject* pGameObject = NULL;

	for (UINT i = 0; i < nObjects; i++)
	{
		(UINT)::fread_s(&xmf3Position, sizeof(XMFLOAT3), sizeof(float), 3, pInFile);
		(UINT)::fread_s(&xmf3Rotation, sizeof(XMFLOAT3), sizeof(float), 3, pInFile); //Euler Angle
		(UINT)::fread_s(&xmf3Scale, sizeof(XMFLOAT3), sizeof(float), 3, pInFile);
		(UINT)::fread_s(&xmf4Rotation, sizeof(XMFLOAT4), sizeof(float), 4, pInFile); //Quaternion

		XMFLOAT4X4 xmmtxWorld = Matrix4x4::Identity();

		(UINT)::fread_s(&xmmtxWorld, sizeof(XMFLOAT4X4), sizeof(XMFLOAT4X4), 1, pInFile);
		pGameObject = new CStaticObject();
		pGameObject->SetChild(pModel, true);

		pGameObject->m_xmf4Rotation = xmf4Rotation;
		pGameObject->m_xmf3Scale = Vector3::ScalarProduct(xmf3Scale, 0.5f, false);
		pGameObject->m_xmf4x4Transform = xmmtxWorld;
		pGameObject->m_iTrackNumber = i;
		pGameObject->UpdateTransform(NULL);
		pGameObject->OnInitialize();
	
		m_pStaticObjLists[index]->emplace_back(pGameObject);
		
	
	}
	if(pInFile) fclose(pInFile);
}

void CGameScene::PlaceDynamicFromFile(CGameObject* pModel, char* FileName, int index,  int _OpenningDoorControl )
{

	FILE* pInFile = NULL;
	::fopen_s(&pInFile, FileName, "rb");
	::rewind(pInFile);

	UINT nObjects = 0;
	(UINT)::fread_s(&nObjects,sizeof(UINT), sizeof(UINT), 1, pInFile);

	XMFLOAT3 xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f),
		xmf3Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f),
		xmf3Scale = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4 xmf4Rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	CDynamicObject* pGameObject = NULL;
	
	//m_pDynamicObjLists[index]->reserve(MAXDYNAMICVEC);
	for (UINT i = 0; i < nObjects; i++)
	{
		(UINT)::fread_s(&xmf3Position, sizeof(XMFLOAT3), sizeof(float), 3, pInFile);
		(UINT)::fread_s(&xmf3Rotation, sizeof(XMFLOAT3), sizeof(float), 3, pInFile); //Euler Angle
		(UINT)::fread_s(&xmf3Scale, sizeof(XMFLOAT3), sizeof(float), 3, pInFile);
		(UINT)::fread_s(&xmf4Rotation, sizeof(XMFLOAT4), sizeof(float), 4, pInFile); //Quaternion

		XMFLOAT4X4 xmmtxWorld;
		(UINT)::fread_s(&xmmtxWorld, sizeof(XMFLOAT4X4), sizeof(XMFLOAT4X4), 1, pInFile);

		switch (index)
		{
		case OBJECT_TYPE_ITEMBOX:
			pGameObject = new CItemBox();
			break;
		case OBJECT_TYPE_DRUGMAKER:
		    pGameObject = new CDrugMaker(i);
			break;
		
		case OBJECT_TYPE_RESEARCHER:
			pGameObject = new CResearcher(i);
			break;
		default:
			pGameObject = new CDynamicObject();
			break;
		}

		pGameObject->m_iTrackNumber = i;
		pGameObject->SetChild(pModel, true);

		pGameObject->m_xmf4Rotation = xmf4Rotation;
		pGameObject->m_xmf3Scale = Vector3::ScalarProduct(xmf3Scale, 0.5f, false);
		pGameObject->m_xmf4x4Transform = xmmtxWorld;

	
		pGameObject->UpdateTransform(NULL);
		if (!_OpenningDoorControl)
		{
			pGameObject->OnInitialize();
			m_pDynamicObjLists[index]->emplace_back(pGameObject);
		}
		else
		{
			for (int i = 0; i < pGameObject->m_pChild->m_nBoundingBoxes; i++)
			{
				pGameObject->m_pChild->m_pBoundingBoxes[i].Update
				(&pGameObject->m_xmf4x4World, &pGameObject->m_xmf4Rotation, &pGameObject->m_xmf3Scale);
			}
		    m_pOpeningWarningDoors[i] = (pGameObject);
			CCollisionMgr::GetInstance()->m_pOpeningDoors[i] = pGameObject;
		}
	}
	if (pInFile) fclose(pInFile);
}

void CGameScene::PlaceMonsterFromFile(CGameObject* pModel, char* FileName, int index,
	ID3D12Device* pd3dDevice,ID3D12GraphicsCommandList* pd3dCommandList)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, FileName, "rb");
	::rewind(pInFile);

	UINT nObjects = 0;
	(UINT)::fread_s(&nObjects, sizeof(UINT), sizeof(UINT), 1, pInFile);

	XMFLOAT3 xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f),
		xmf3Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f),
		xmf3Scale = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4 xmf4Rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);


	CTexture* pMonsterHPTex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pMonsterHPTex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UserInterface/HP/MonsterHP.dds", 0);
	CShader* pShder = new CShader_MonsterHP();
	
	pShder->CreateShader(pd3dDevice, GetGraphicsRootSignature(), FINAL_MRT_COUNT);
	pShder->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShder->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pMonsterHPTex, ROOT_PARAMETER_ALBEDO_TEX, 0);

	CMonster* pGameObject = NULL;
	m_pMonsterLists[index]->reserve(MAXMONSTERVEC);

	for (UINT i = 0; i < nObjects; i++)
	{
		(UINT)::fread_s(&xmf3Position, sizeof(XMFLOAT3), sizeof(float), 3, pInFile);
		(UINT)::fread_s(&xmf3Rotation, sizeof(XMFLOAT3), sizeof(float), 3, pInFile); //Euler Angle
		(UINT)::fread_s(&xmf3Scale, sizeof(XMFLOAT3), sizeof(float), 3, pInFile);
		(UINT)::fread_s(&xmf4Rotation,sizeof(XMFLOAT4), sizeof(float), 4, pInFile); //Quaternion

		XMFLOAT4X4 xmmtxWorld;
		(UINT)::fread_s(&xmmtxWorld, sizeof(XMFLOAT4X4), sizeof(XMFLOAT4X4), 1, pInFile);

		float scale = xmf3Scale.x;
		switch (index)
		{

		case MONSTER_TYPE_YANGMAL:
			pGameObject = new CYangmal();
			pGameObject->SetChild(pModel, true);
			pGameObject->m_iTrackNumber = i;
			pGameObject->Initialize(XMFLOAT3(377, 39, 118), 5);
			scale /= 15.f;
			break;
		case MONSTER_TYPE_TOSM:
			pGameObject = new CTosm();
			pGameObject->SetChild(pModel, true);
			pGameObject->m_iTrackNumber = i;
			pGameObject->Initialize(XMFLOAT3(206, 83, 138), 5);
			scale /= 1.5f;
			break;
		default:
			pGameObject = new CMonster();
			break;
		}
		pGameObject->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		CUI_MonsterHP* hp = new CUI_MonsterHP();
	
		hp->SetShader(pShder);
		hp->SetTexture(pMonsterHPTex);

		hp->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		pGameObject->SetHPUI(hp);

		XMFLOAT2 SCALE = XMFLOAT2(scale, scale / 10);
		hp->SetInstanceInfo(SCALE, pd3dDevice, pd3dCommandList);

		pGameObject->m_xmf4Rotation = xmf4Rotation;
		pGameObject->m_xmf3Scale =  Vector3::ScalarProduct(xmf3Scale, 1.8f, false);
		pGameObject->m_xmf4x4Transform = xmmtxWorld;
		if (index == MONSTER_TYPE_YANGMAL)
			pGameObject->SetScale(2, 2, 2);
		else
			pGameObject->SetScale(1.5f, 1.5f, 1.5f);

		pGameObject->UpdateTransform(NULL);
		pGameObject->OnInitialize();
		m_pMonsterLists[index]->emplace_back(pGameObject);
		//m_pMonsterLists[index]->
	}
	//pModel->m_pAnimationController = NULL;
	if (pInFile) fclose(pInFile);
}



void CGameScene::ChangeTerrainPipeline()
{
	m_bPipelineStateIndex = (!m_bPipelineStateIndex);
}
