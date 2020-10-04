//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "Material.h"
#include "Animation.h"
#include "Shader_CollisionBox.h"
//#include ""

int ReadIntegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

//=========================================================================================================================
CGameObject::CGameObject()
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObject::~CGameObject()
{
	if (m_pMesh)
		m_pMesh->Release();

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{

			if (m_ppMaterials[i])
			{
				m_ppMaterials[i]->ReleaseUploadBuffers();
				m_ppMaterials[i]->Release();

			}
		}
	}
	if (m_ppMaterials) delete[] m_ppMaterials;
}

void CGameObject::AddRef()
{
	m_nReferences++;

	if (m_pSibling) m_pSibling->AddRef();
	if (m_pChild) m_pChild->AddRef();
}

void CGameObject::Release()
{
	if (m_pChild) m_pChild->Release();
	if (m_pSibling) m_pSibling->Release();

	if (m_pAnimationController)
	{
		delete m_pAnimationController;
		m_pAnimationController = NULL;
	}

	if (m_pCollisionBoxShader)
	{
		m_pCollisionBoxShader->Release();
		m_pCollisionBoxShader = NULL;
	}

	if (m_pBoundingBoxes)
	{
		for(int i =0; i<m_nBoundingBoxes; i++)
			m_pBoundingBoxes[i].Release();
		delete[] m_pBoundingBoxes;
		m_pBoundingBoxes = NULL;
	}

	if (--m_nReferences <= 0) delete this;
}

CCollisionBox* CGameObject::GetCollisionBoxes()
{
	if (m_pBoundingBoxes) return m_pBoundingBoxes;
	if (m_pSibling) return m_pSibling->GetCollisionBoxes();
	if (m_pChild)	return m_pChild->GetCollisionBoxes();

	return NULL;
}

XMFLOAT4 CGameObject::GetRotateQuaternion(float Scale, XMFLOAT4X4& world)
{
	XMFLOAT4X4 RotateMat;
	XMFLOAT4X4 inverse = Matrix4x4::Identity();

	inverse._41 = world._41;
	inverse._42 = world._42;
	inverse._43 = world._43;
	inverse = Matrix4x4::Inverse(inverse);
	RotateMat = Matrix4x4::Multiply(world, inverse);
	inverse = Matrix4x4::Identity();

	XMFLOAT4X4 mtxScale = Matrix4x4::Inverse(Matrix4x4::Scale(inverse, Scale));
	RotateMat = Matrix4x4::Multiply(mtxScale, RotateMat);
	XMFLOAT4 Quart;
	XMStoreFloat4(&Quart, XMQuaternionRotationMatrix(XMLoadFloat4x4(&RotateMat)));

	return Quart;
}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

CMesh* CGameObject::GetMesh()
{
	if (m_pMesh)
		return m_pMesh;

	if (m_pChild) return m_pChild->GetMesh();
	if (m_pSibling) return m_pSibling->GetMesh();

	return NULL;
	//return m_pMesh;
}

void CGameObject::SetShader(CShader* pShader, int nTextures)
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial * [m_nMaterials];
	m_ppMaterials[0] = new CMaterial(nTextures);
	m_ppMaterials[0]->SetShader(pShader);
}

void CGameObject::CreateMaterial()
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial * [m_nMaterials];
	m_ppMaterials[0] = new CMaterial(1);
}

void CGameObject::SetShader(int nMaterial, CShader* pShader)
{
	if (m_ppMaterials[nMaterial])
		m_ppMaterials[nMaterial]->SetShader(pShader);
}

void CGameObject::SetMaterial(int nMaterial, CMaterial* pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

void CGameObject::SetParentRenderState(bool bRender)
{
	m_bRender = bRender;
	if (m_pParent) m_pParent->SetParentRenderState(bRender);
}

void CGameObject::UpdateAnimationCache(int iNum)
{
	if (m_pAnimationController)
		m_pAnimationController->UpdateTransformCache(iNum);
	if (m_pSibling)	m_pSibling->UpdateAnimationCache(iNum);
	if (m_pChild)	m_pChild->UpdateAnimationCache(iNum);
}

void CGameObject::SetChild(CGameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;

	UpdateTransform(NULL); // 포지션이 변한 상태를 그대로 월드행렬에 적용 
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	m_xmf4x4Transform._41 = xmf3Position.x;
	m_xmf4x4Transform._42 = xmf3Position.y;
	m_xmf4x4Transform._43 = xmf3Position.z;

	UpdateTransform(NULL); // 포지션이 변한 상태를 그대로 월드행렬에 적용 
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::SetTexture(CTexture* tex)
{
	if (m_ppMaterials)
		m_ppMaterials[0]->SetTexture(tex);
	if (m_pChild) m_pChild->SetTexture(tex);
	if (m_pSibling) m_pSibling->SetTexture(tex);
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);
	UpdateTransform(NULL);

	if (XMMatrixIsNaN(XMLoadFloat4x4(&m_xmf4x4World)))
		int a = 0;
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent, int iNum)
{
	OnPrepareRender();

	if (m_pAnimationController) 
		m_pAnimationController->AdvanceTime(fTimeElapsed, NULL, iNum);
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed, pxmf4x4Parent, iNum);
	if (m_pChild) m_pChild->Animate(fTimeElapsed, &m_xmf4x4World, iNum);
}

CGameObject* CGameObject::FindFrame(char* pstrFrameName)
{
	CGameObject* pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::SetAnimatationSpeed(int iSet, float fSpeed)
{
	if (m_pAnimationController)
		m_pAnimationController->m_pAnimationSets[iSet].m_fSpeed = fSpeed;
}

void CGameObject::OnPrepareRender() {}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{

	if (!m_bRender) return;
	if (m_pBoundingBoxes && nPipelineState == 0 && m_nBoundingBoxes > 0)
	{
		int CullCount = 0;
		//m_pCollisionBoxShader->Render(pd3dCommandList, pCamera);
		for (int i = 0; i < m_nBoundingBoxes; i++)
		{
			m_pBoundingBoxes[i].Update(&m_xmf4x4World);
			//m_pBoundingBoxes[i].Render(pd3dCommandList, pCamera, &m_xmf4x4World);
			if (!pCamera->m_boundingFrustum.Intersects(m_pBoundingBoxes[i].boundingBox))
			{	
				CullCount++;
			}
		}/*
		if (CullCount >= m_nBoundingBoxes)
		{
			SetParentRenderState(false);
			return;
		}*/
	}
	if (nPipelineState == 1)
	{
		XMFLOAT3 Camerapos = pCamera->m_boundingFrustum.Origin;
		float distance = Vector3::Length(Vector3::Subtract(Camerapos, GetPosition()));
		if (distance > 200) return;
	}
	OnPrepareRender();
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_pAnimationController)
		m_pAnimationController->UpdateShaderVariables(pd3dCommandList,m_iTrackNumber);

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader)
					m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);

				if(nPipelineState != 1)
					m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);

			}
			if (m_pMesh)
				m_pMesh->Render(pd3dCommandList, i);
		}
	}
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera, nPipelineState);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera, nPipelineState);

}

CMaterialColors::CMaterialColors(MATERIALLOADINFO* pMaterialInfo)
{
	m_xmf4Diffuse = pMaterialInfo->m_xmf4AlbedoColor;
	m_xmf4Specular = pMaterialInfo->m_xmf4SpecularColor; //(r,g,b,a=power)
	m_xmf4Specular.w = (pMaterialInfo->m_fGlossiness * 255.0f);
	m_xmf4Emissive = pMaterialInfo->m_xmf4EmissiveColor;
}

CTexture* CGameObject::FindReplicatedTexture(_TCHAR* pstrTextureName)
{
	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i])
		{
			for (int j = 0; j < m_ppMaterials[i]->m_nTextures; j++)
			{
				if (m_ppMaterials[i]->m_ppTextures[j])
				{
					if (!_tcsncmp(m_ppMaterials[i]->m_ppstrTextureNames[j], pstrTextureName, _tcslen(pstrTextureName))) return(m_ppMaterials[i]->m_ppTextures[j]);
				}
			}
		}
	}
	CTexture* pTexture = NULL;
	if (m_pSibling) if (pTexture = m_pSibling->FindReplicatedTexture(pstrTextureName)) return(pTexture);
	if (m_pChild) if (pTexture = m_pChild->FindReplicatedTexture(pstrTextureName)) return(pTexture);

	return(NULL);
}

void CGameObject::UpdateTrackNumber(int iNum)
{
	m_iTrackNumber = iNum;
	if (m_pSibling)	m_pSibling->UpdateTrackNumber(iNum);
	if (m_pChild)	m_pChild->UpdateTrackNumber(iNum);
}

//=========================================================================================================================
void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) {}

void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	m_pd3dcbGameObjects = NULL;
}

void CGameObject::UpdateObjectID(int ID)
{
	m_ObjectID = ID;
	if (m_pChild) m_pChild->UpdateObjectID(ID);
	if (m_pSibling) m_pSibling->UpdateObjectID(ID);
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

	UINT getobjectID = m_ObjectID;
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 4, &getobjectID, 16);
}

//void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* pMaterial) {}

void CGameObject::UpdateCollisionBoxes(XMFLOAT4X4* world)
{
	if (m_nBoundingBoxes > 0)
	{
		for (int i = 0; i < m_nBoundingBoxes; i++)
		{
			if (world)
				m_pBoundingBoxes[i].Update(world);
		}
	}
	if (m_pSibling) m_pSibling->UpdateCollisionBoxes(&m_xmf4x4World);
	if (m_pChild) m_pChild->UpdateCollisionBoxes(&m_xmf4x4World);

}


void CGameObject::UpdateCollisionBoxes(XMFLOAT4X4* world, XMFLOAT4* ParentOrientation, XMFLOAT3* ParentScale)
{
	if (m_nBoundingBoxes > 0)
	{
		for (int i = 0; i < m_nBoundingBoxes; i++)
		{
			if (world)
				m_pBoundingBoxes[i].Update(world, ParentOrientation,ParentScale);
		}
	}
	if (m_pSibling) m_pSibling->UpdateCollisionBoxes(&m_xmf4x4World, ParentOrientation ,ParentScale);
	if (m_pChild) m_pChild->UpdateCollisionBoxes(&m_xmf4x4World, ParentOrientation, ParentScale);


}
void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i]) m_ppMaterials[i]->ReleaseUploadBuffers();
	}


	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

//=========================================================================================================================
CGameObject* CGameObject::GetRootSkinnedGameObject()
{
	if (m_pAnimationController) return(this);

	CGameObject* pRootSkinnedGameObject = NULL;
	if (m_pSibling) if (pRootSkinnedGameObject = m_pSibling->GetRootSkinnedGameObject()) return(pRootSkinnedGameObject);
	if (m_pChild) if (pRootSkinnedGameObject = m_pChild->GetRootSkinnedGameObject()) return(pRootSkinnedGameObject);

	return(NULL);
}

void CGameObject::CacheSkinningBoneFrames(CGameObject* pRootFrame)
{
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT))
	{
		CSkinnedMesh* pSkinnedMesh = (CSkinnedMesh*)m_pMesh;
		for (int i = 0; i < pSkinnedMesh->m_nSkinningBones; i++)
		{
			pSkinnedMesh->m_ppSkinningBoneFrameCaches[i] = pRootFrame->FindFrame(pSkinnedMesh->m_ppstrSkinningBoneNames[i]);
#ifdef _WITH_DEBUG_SKINNING_BONE
			TCHAR pstrDebug[256] = { 0 };
			TCHAR pwstrBoneCacheName[64] = { 0 };
			TCHAR pwstrSkinningBoneName[64] = { 0 };
			size_t nConverted = 0;
			mbstowcs_s(&nConverted, pwstrBoneCacheName, 64, pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_pstrFrameName, _TRUNCATE);
			mbstowcs_s(&nConverted, pwstrSkinningBoneName, 64, pSkinnedMesh->m_ppstrSkinningBoneNames[i], _TRUNCATE);
			_stprintf_s(pstrDebug, 256, _T("SkinningBoneFrame:: Cache(%s) Bone(%s)\n"), pwstrBoneCacheName, pwstrSkinningBoneName);
			OutputDebugString(pstrDebug);
#endif
		}
	}
	if (m_pSibling) m_pSibling->CacheSkinningBoneFrames(pRootFrame);
	if (m_pChild) m_pChild->CacheSkinningBoneFrames(pRootFrame);
}

void CGameObject::SetAnimationSet(int nAnimationSet, int iNum)
{
	if (m_pAnimationController) m_pAnimationController->SetAnimationSet(nAnimationSet, iNum);

	if (m_pSibling) m_pSibling->SetAnimationSet(nAnimationSet, iNum);
	if (m_pChild) m_pChild->SetAnimationSet(nAnimationSet, iNum);
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;
	//인수가 넘어오면 변환행렬에 인수를 곱하고, 인수가 없으면 변환행렬을 그대로 적용한다
	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World); // 자식에게 부모의 변환정보를 적용시킨다
}

//#define _WITH_DEBUG_FRAME_HIERARCHY
CGameObject* CGameObject::LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader, int iHasAnimations)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);


	CGameObject* pGameObject = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader);

	pGameObject->CacheSkinningBoneFrames(pGameObject);

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, "Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject, NULL);
#endif

	if (iHasAnimations>0)
	{
		pGameObject->m_pAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, pGameObject, iHasAnimations);
		pGameObject->LoadAnimationFromFile(pInFile);
		pGameObject->m_pAnimationController->SetAnimationSet(0);
	}

	pGameObject->LoadBoundingBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile);
	if(pInFile) fclose(pInFile);
	return(pGameObject);
}

void CGameObject::LoadBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pGameObject, FILE* pInFile)
{
	BYTE nStrLength = 0;
	char pstrToken[80] = { '\0' };

	(UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	(UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);

	pstrToken[nStrLength] = '\0';
	if (!strcmp(pstrToken, "</AnimationSets>"))
	{
		(UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		(UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);

		pstrToken[nStrLength] = '\0';
	}
	if (!strcmp(pstrToken, "<Bounds>:"))
	{
		m_pCollisionBoxShader = new Shader_CollisionBox();

		(UINT)::fread(&pGameObject->m_nBoundingBoxes, sizeof(int), 1, pInFile);
		pGameObject->m_pBoundingBoxes = new CCollisionBox[pGameObject->m_nBoundingBoxes];

		GS_COLLISION_BOX_INFO* boxes = new GS_COLLISION_BOX_INFO[pGameObject->m_nBoundingBoxes];

		for (int i = 0; i < pGameObject->m_nBoundingBoxes; i++)
		{
			(UINT)::fread(&boxes[i].m_xmf3Center, sizeof(float), 3, pInFile);
			pGameObject->m_pBoundingBoxes[i].m_Center = boxes[i].m_xmf3Center;

			(UINT)::fread(&boxes[i].m_xmf3Extent, sizeof(float), 3, pInFile);
			pGameObject->m_pBoundingBoxes[i].m_Extents = boxes[i].m_xmf3Extent;

			(UINT)::fread(&boxes[i].m_xmf4Orientation, sizeof(float), 4, pInFile);
			pGameObject->m_pBoundingBoxes[i].m_Orientation = boxes[i].m_xmf4Orientation;

			m_pBoundingBoxes[i].BuildBuffer(pd3dDevice, pd3dCommandList, NULL);
		}
		m_pCollisionBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
		m_pCollisionBoxShader->BuildObjects(pd3dDevice, pd3dCommandList);
	}


}

CGameObject* CGameObject::LoadFrameHierarchyFromFile
(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, 
	FILE* pInFile, CShader* pShader)
{
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	CGameObject* pGameObject = NULL;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			pGameObject = new CGameObject();

			nReads = (UINT)::fread(&nFrame, sizeof(int), 1, pInFile);
			nReads = (UINT)::fread(&nTextures, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pGameObject->m_pstrFrameName, sizeof(char), nStrLength, pInFile);
			pGameObject->m_pstrFrameName[nStrLength] = '\0';
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion

		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4Transform, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			// 애니메이션 있는거는 안들어온다. 
			CStandardMesh* pMesh = new CStandardMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pGameObject->SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			CSkinnedMesh* pSkinnedMesh = new CSkinnedMesh(pd3dDevice, pd3dCommandList);
			pSkinnedMesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

			pSkinnedMesh->LoadSkinInfoFromFile(pd3dDevice, pd3dCommandList, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile); //<Mesh>:
			pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			pGameObject->SetMesh(pSkinnedMesh);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = 0;
			nReads = (UINT)::fread(&nChilds, sizeof(int), 1, pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject* pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, "(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

void CGameObject::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	CGameObject* pParent, FILE* pInFile, CShader* pShader)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	BYTE nStrLength = 0;

	UINT nReads = (UINT)::fread(&m_nMaterials, sizeof(int), 1, pInFile);

	m_ppMaterials = new CMaterial * [m_nMaterials];
	for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;

	CMaterial* pMaterial = NULL;
	//m_pCollisionBoxShader = NULL;

	//Shader_CollisionBox* pCollisionShader = NULL; 

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);

			pMaterial = new CMaterial(7);
			//0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

			if (!pShader)
			{
				UINT nMeshType = GetMeshType();
				if (nMeshType & VERTEXT_NORMAL_TANGENT_TEXTURE)
				{
					if (nMeshType & VERTEXT_BONE_INDEX_WEIGHT)
					{
						pMaterial->SetSkinnedAnimationShader();
					}
					else
					{
						pMaterial->SetStandardShader();
					}
				}
			}
			else
			{
				pMaterial->SetShader(pShader);
			}

			SetMaterial(nMaterial, pMaterial);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_ALBEDO_MAP, ROOT_PARAMETER_ALBEDO_TEX, pMaterial->m_ppstrTextureNames[0], &(pMaterial->m_ppTextures[0]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_SPECULAR_MAP, ROOT_PARAMETER_SPECULAR_TEX, pMaterial->m_ppstrTextureNames[1], &(pMaterial->m_ppTextures[1]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_NORMAL_MAP, ROOT_PARAMETER_NORMAL_TEX, pMaterial->m_ppstrTextureNames[2], &(pMaterial->m_ppTextures[2]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_METALLIC_MAP, ROOT_PARAMETER_METALLIC_TEX, pMaterial->m_ppstrTextureNames[3], &(pMaterial->m_ppTextures[3]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_EMISSION_MAP, ROOT_PARAMETER_EMISSION_TEX, pMaterial->m_ppstrTextureNames[4], &(pMaterial->m_ppTextures[4]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}

void CGameObject::LoadAnimationFromFile(FILE* pInFile)
{
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;
	int nAnimations = 0;
	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			nReads = (UINT)::fread(&m_pAnimationController->m_nAnimationSets, sizeof(int), 1, pInFile);

			m_pAnimationController->m_pAnimationSets = new CAnimationSet[m_pAnimationController->m_nAnimationSets];
		}
		else if (!strcmp(pstrToken, "<FrameNames>:"))
		{
			nReads = (UINT)::fread(&m_pAnimationController->m_nAnimationBoneFrames, sizeof(int), 1, pInFile);
			m_pAnimationController->m_ppAnimationBoneFrameCaches = new CGameObject * [m_pAnimationController->m_nAnimationBoneFrames];

			for (int i = 0; i < m_pAnimationController->m_nAnimationBoneFrames; i++)
			{
				nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
				nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
				pstrToken[nStrLength] = '\0';

				m_pAnimationController->m_ppAnimationBoneFrameCaches[i] = FindFrame(pstrToken);

#ifdef _WITH_DEBUG_SKINNING_BONE
				TCHAR pstrDebug[256] = { 0 };
				TCHAR pwstrAnimationBoneName[64] = { 0 };
				TCHAR pwstrBoneCacheName[64] = { 0 };
				size_t nConverted = 0;
				mbstowcs_s(&nConverted, pwstrAnimationBoneName, 64, pstrToken, _TRUNCATE);
				mbstowcs_s(&nConverted, pwstrBoneCacheName, 64, m_pAnimationSets->m_ppAnimationBoneFrameCaches[i]->m_pstrFrameName, _TRUNCATE);
				_stprintf_s(pstrDebug, 256, _T("AnimationBoneFrame:: Cache(%s) AnimationBone(%s)\n"), pwstrBoneCacheName, pwstrAnimationBoneName);
				OutputDebugString(pstrDebug);
#endif
			}
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			nAnimations++;
			int nAnimationSet = 0;
			nReads = (UINT)::fread(&nAnimationSet, sizeof(int), 1, pInFile);
			CAnimationSet* pAnimationSet = &m_pAnimationController->m_pAnimationSets[nAnimationSet];

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pAnimationSet->m_pstrName, sizeof(char), nStrLength, pInFile);
			pAnimationSet->m_pstrName[nStrLength] = '\0';

			nReads = (UINT)::fread(&pAnimationSet->m_fLength, sizeof(float), 1, pInFile);
			nReads = (UINT)::fread(&pAnimationSet->m_nFramesPerSecond, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&pAnimationSet->m_nKeyFrameTransforms, sizeof(int), 1, pInFile);
			pAnimationSet->m_pfKeyFrameTransformTimes = new float[pAnimationSet->m_nKeyFrameTransforms];
			pAnimationSet->m_ppxmf4x4KeyFrameTransforms = new XMFLOAT4X4 * [pAnimationSet->m_nKeyFrameTransforms];
			for (int i = 0; i < pAnimationSet->m_nKeyFrameTransforms; i++) pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i] = new XMFLOAT4X4[m_pAnimationController->m_nAnimationBoneFrames];

			for (int i = 0; i < pAnimationSet->m_nKeyFrameTransforms; i++)
			{
				nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
				nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
				pstrToken[nStrLength] = '\0';

				if (!strcmp(pstrToken, "<Transforms>:"))
				{
					int nKeyFrame = 0;
					nReads = (UINT)::fread(&nKeyFrame, sizeof(int), 1, pInFile); //i

					nReads = (UINT)::fread(&pAnimationSet->m_pfKeyFrameTransformTimes[i], sizeof(float), 1, pInFile);
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i], sizeof(float), 
						16 * m_pAnimationController->m_nAnimationBoneFrames, pInFile);
				}
			}
#ifdef _WITH_ANIMATION_SRT
			nReads = (UINT)::fread(&pAnimationSet->m_nKeyFrameTranslations, sizeof(int), 1, pInFile);
			pAnimationSet->m_pfKeyFrameTranslationTimes = new float[pAnimationSet->m_nKeyFrameTranslations];
			pAnimationSet->m_ppxmf3KeyFrameTranslations = new XMFLOAT3 * [pAnimationSet->m_nKeyFrameTranslations];
			for (int i = 0; i < pAnimationSet->m_nKeyFrameTranslations; i++) pAnimationSet->m_ppxmf3KeyFrameTranslations[i] = new XMFLOAT3[m_pAnimationController->m_nAnimationBoneFrames];
#endif
		}
		else
		{
			break;
		}
		if (nAnimations == m_pAnimationController->m_nAnimationSets )
			break;
	}
}

void CGameObject::PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) CGameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) CGameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

CCollisionBox::CCollisionBox()
{
	m_Center = {0.0f, 0.0f, 0.0f};
	m_Extents = { 0.0f, 0.0f, 0.0f };
	m_Orientation = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_iBoneIndex = 0 ;

	m_xmf4x4World = Matrix4x4::Identity();;
	m_pParent = NULL;

	m_pd3dCollisionBuffer = NULL;
	m_pd3dCollisionUploadBuffer = NULL;
}

CCollisionBox::~CCollisionBox()
{
}

void CCollisionBox::Update(XMFLOAT4X4* Parentworld, XMFLOAT4* ParentOrientation, XMFLOAT3* ParentScale)
{
	XMFLOAT4X4 world;
	if (m_pParent)
		world = m_pParent->m_xmf4x4World;
	else
		world = *Parentworld;

	XMFLOAT4 Position = XMFLOAT4(m_Center.x, m_Center.y, m_Center.z, 1);
	Position = Vector4::MultiflyMATRIX(Position, world);
	
	XMFLOAT4 orientation = m_Orientation;
	
	if (ParentOrientation) orientation = *ParentOrientation;
	
	if (ParentScale) boundingBox.Extents = Vector3::Multiply(m_Extents, *ParentScale);

	boundingBox.Orientation = orientation;
	
	boundingBox.Center = XMFLOAT3(Position.x, Position.y, Position.z);
	
}

void CCollisionBox::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* ParentWorld)
{
	if (m_pParent)
		m_xmf4x4World = m_pParent->m_xmf4x4World;
	else
		m_xmf4x4World = *ParentWorld;

	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

}

void CCollisionBox::BuildBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{

	GS_COLLISION_BOX_INFO info;
	info.m_xmf3Center = m_Center;
	info.m_xmf3Extent = boundingBox.Extents = m_Extents;
	info.m_xmf4Orientation = m_Orientation;
	m_pd3dCollisionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, &info,
		sizeof(GS_COLLISION_BOX_INFO), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dCollisionUploadBuffer);

	m_d3dCollisionBufferView.BufferLocation = m_pd3dCollisionBuffer->GetGPUVirtualAddress();
	m_d3dCollisionBufferView.StrideInBytes = sizeof(GS_COLLISION_BOX_INFO);
	m_d3dCollisionBufferView.SizeInBytes = sizeof(GS_COLLISION_BOX_INFO);
}

void CCollisionBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, XMFLOAT4X4* ParentWorld)
{
	UpdateShaderVariables(pd3dCommandList, ParentWorld);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[1] = { m_d3dCollisionBufferView };
	pd3dCommandList->IASetVertexBuffers(0, _countof(pVertexBufferViews), pVertexBufferViews);
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	pd3dCommandList->DrawInstanced(1, 1, 0, 0);
}

void CCollisionBox::Release()
{
	if (m_pd3dCollisionUploadBuffer) 
		m_pd3dCollisionUploadBuffer->Release();

	if (m_pd3dCollisionBuffer)
		m_pd3dCollisionBuffer->Release();

	//if (m_pParent)m_pParent->Release();
}
