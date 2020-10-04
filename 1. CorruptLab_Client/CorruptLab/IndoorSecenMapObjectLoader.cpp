#include "stdafx.h"
#include "Scene_Game2.h"
#include "Object_StaticObj.h"
#include "Mgr_Collision.h"
#include "Shader_Standard.h"
#include "Object_DrugMaker.h"
#include "Mgr_Scene.h"
#include "Object_Door.h"
#include "Obect_Desk.h"


#define MAXSTATICEVEC 100 
#define MAXDYNAMICVEC 100
#define MAXMONSTERVEC 100 

class DrugMakersImpormation;

void CGameScene2::PlaceObjectsFromFile(ID3D12Device* pd3dDevice,
	ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList)
{

	m_nStaticObjectTypeNum = 15;
	m_pStaticObjLists = new vector<CGameObject*> * [m_nStaticObjectTypeNum];

	for (int i = 0; i < m_nStaticObjectTypeNum; i++)
		m_pStaticObjLists[i] = new vector<CGameObject*>();

	m_nDynamicObjectTypeNum = 4;
	m_pDynamicObjLists = new vector<CDynamicObject*> * [m_nDynamicObjectTypeNum];

	for (int i = 0; i < m_nDynamicObjectTypeNum; i++)
		m_pDynamicObjLists[i] = new vector<CDynamicObject*>();

	int CurrentIndex = 0;
	CGameObject* pDiverObject = NULL;

    // Static ================================================================================
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile  // [ Desk02 ]
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Desk2.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/Desk02s.bin", OBJECT_INDOOR_TYPE_DESK);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile  // [ Doggy ]
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Doggy.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/DoggyT.bin", OBJECT_INDOOR_TYPE_DOGGY);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile  // [ Duggy ]
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Duggy.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/DuggyT.bin", OBJECT_INDOOR_TYPE_DUGGY);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile  // [ Mouse ]
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Mouse.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/MouseT.bin", OBJECT_INDOOR_TYPE_MOUSE);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile  // [ Cat ] 
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Cat.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/CatT.bin", OBJECT_INDOOR_TYPE_CAT);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile  // [ Granny ] 
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/granny.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/GrannyT.bin", OBJECT_INDOOR_TYPE_GRANNY);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile  // [ Tosm ] 
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Tosm.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/TosmT.bin", OBJECT_INDOOR_TYPE_TOSM);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile  // [ Yangmal ] 
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/YangmalIndoor.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/YangmalTF.bin", OBJECT_INDOOR_TYPE_YANGMAL);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile  // [ Chair ] 
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Chair.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/Chairs.bin", OBJECT_INDOOR_TYPE_CHAIR);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile  // [ board ] 
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Board.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/Boards.bin", OBJECT_INDOOR_TYPE_BOARD);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile  // [ door ] 
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Door.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/Doors.bin", OBJECT_INDOOR_TYPE_DOOR);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile 
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Number.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/passwordT.bin", OBJECT_INDOOR_TYPE_PASSWORD);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/AnimalAll.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/AnimalAllT.bin", OBJECT_INDOOR_TYPE_ANIMALALL);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/BossMachine.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/BossMachineT.bin", OBJECT_INDOOR_TYPE_BOSSMACHINE);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/BossMachineMonitor.bin", NULL, 0);
	PlaceStaticObjectsFromFile(pDiverObject, "ObjectsData/BossMachineMonitorT.bin", OBJECT_INDOOR_TYPE_BOSSMONITER);


	// Dynamic ================================================================================
	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/LaboratoryObj.bin", NULL, 0);
	CGameObject* Alpha = pDiverObject->FindFrame("Object021");
	CShader* alphaShader = new CIndoorSceneTransparentedStandardShader();
	alphaShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	Alpha->SetShader(0, alphaShader);
	PlaceDynamicFromFile(pDiverObject, "ObjectsData/Laboratorys.bin", OBJECT_INDOOR_TYPE_LABORATOR);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, 
		pd3dGraphicsRootSignature, "Model/DrugMaker.bin", NULL, 0); //[ DrugMakers ]
	CGameObject* IndoorDrugMakerAlpha = pDiverObject->FindFrame("cylinderAlpha");
	CShader* IndoorDrugMakerShader = new CTransparentedStandardShader();
	IndoorDrugMakerShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	IndoorDrugMakerAlpha->SetShader(0, IndoorDrugMakerShader);

	CGameObject* bosscylinder = new CStaticObject();
	bosscylinder->SetChild(Alpha, true);
	bosscylinder->SetPosition(0, -7, 0);
	bosscylinder->SetScale(10,10,10);
	bosscylinder->UpdateTransform(NULL);
	bosscylinder->OnInitialize();
	m_pStaticObjLists[OBJECT_INDOOR_TYPE_BOSSMONITER]->emplace_back(bosscylinder);

	PlaceDynamicFromFile(pDiverObject,"ObjectsData/LabatoryIndoorT.bin" , OBJECT_INDOOR_TYPE_DRUGMAKER);


	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Door_Opening.bin", NULL, 1);
	PlaceDynamicFromFile(pDiverObject, "ObjectsData/Door_OpeningT.bin", OBJECT_INDOOR_TYPE_DOOROPEN);

	pDiverObject = CGameObject::LoadGeometryAndAnimationFromFile
	(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Desk_Opening.bin", NULL, 8);
	PlaceDynamicFromFile(pDiverObject, "ObjectsData/OpeningDeskT.bin", OBJECT_INDOOR_TYPE_DESKOPEN);

}

void CGameScene2::PlaceStaticObjectsFromFile(CGameObject* pModel, char* FileName, UINT index)
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

	CGameObject* pGameObject = NULL;

	m_pStaticObjLists[index]->reserve(MAXSTATICEVEC);
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

		if (index == OBJECT_INDOOR_TYPE_BOSSMONITER)
			xmmtxWorld._42 -= 1.f;
		pGameObject->m_xmf4Rotation = xmf4Rotation;
		pGameObject->m_xmf3Scale = Vector3::ScalarProduct(xmf3Scale, 0.5f, false);
		pGameObject->m_xmf4x4Transform = xmmtxWorld;
		pGameObject->m_iTrackNumber = i;


		pGameObject->UpdateTransform(NULL);
		pGameObject->OnInitialize();
		m_pStaticObjLists[index]->emplace_back(pGameObject);
	}
	if (pInFile) fclose(pInFile);
}

void CGameScene2::PlaceDynamicFromFile(CGameObject* pModel, char* FileName, int index)
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

	CDynamicObject* pGameObject = NULL;

	m_pDynamicObjLists[index]->reserve(MAXDYNAMICVEC);
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
		case OBJECT_INDOOR_TYPE_DOOROPEN:
			pGameObject = new CDoor();
			pGameObject->m_iTrackNumber = i;
			pGameObject->SetChild(pModel, true);
			break; 

		case OBJECT_INDOOR_TYPE_LABORATOR:
			pGameObject = new CDynamicObject();
			pGameObject->m_iTrackNumber = i;
			pGameObject->SetChild(pModel, true);
			m_DrugmakerImpromation.push_back(DrugMakersImpormation(xmf3Position, xmf3Scale));
			//m_DrugSize.push_back(xmf3Scale.y);
			break;
		case OBJECT_INDOOR_TYPE_DESKOPEN:
			pGameObject = new CDesk();
			pGameObject->m_iTrackNumber = i;
			pGameObject->SetChild(pModel, true);
			break;
		default:
			pGameObject = new CDynamicObject();
			pGameObject->m_iTrackNumber = i;
			pGameObject->SetChild(pModel, true);
			break;
		}

		pGameObject->m_xmf4Rotation = xmf4Rotation;
		pGameObject->m_xmf3Scale = Vector3::ScalarProduct(xmf3Scale, 0.5f, false);
		pGameObject->m_xmf4x4Transform = xmmtxWorld;


		pGameObject->UpdateTransform(NULL);
		pGameObject->OnInitialize();
		m_pDynamicObjLists[index]->emplace_back(pGameObject);
	}
	if (pInFile) fclose(pInFile);
}

void CGameScene2::PlaceMonsterFromFile(CGameObject* pModel, char* FileName, int index,
	ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameScene2::ExcuteAnimationDoor()
{
	for (auto& a : *m_pDynamicObjLists[OBJECT_INDOOR_TYPE_DOOROPEN])
		(dynamic_cast<CDoor*>(a))->Open();
}

void CGameScene2::ExcuteAnimationDesk(int _TrackNumber)
{
	for (auto& a : *m_pDynamicObjLists[OBJECT_INDOOR_TYPE_DESKOPEN])
	{
		if(a->m_iTrackNumber == _TrackNumber)
		(dynamic_cast<CDesk*>(a))->Open();
	}
}
