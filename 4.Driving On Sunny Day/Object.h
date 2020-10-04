//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------
#pragma once
#include "Mesh.h"
#include "Camera.h"

#define DIR_FORWARD		0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#define MATERIAL_ALBEDO_MAP			         0x01
#define MATERIAL_SPECULAR_MAP		     0x02
#define MATERIAL_NORMAL_MAP			     0x04
#define MATERIAL_METALLIC_MAP		         0x08
#define MATERIAL_EMISSION_MAP	          	 0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	 0x20
#define MATERIAL_DETAIL_NORMAL_MAP	 0x40

#define RESOURCE_TEXTURE2D			     0x01
#define RESOURCE_TEXTURE2D_ARRAY	 0x02	//[]
#define RESOURCE_TEXTURE2DARRAY	 0x03
#define RESOURCE_TEXTURE_CUBE		     0x04
#define RESOURCE_BUFFER				         0x05

class CShader;

struct CB_GAMEOBJECT_INFO { XMFLOAT4X4	m_xmf4x4World; };



struct MATERIALLOADINFO
{
	XMFLOAT4 m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	float	 m_fGlossiness = 0.0f;
	float	 m_fSmoothness = 0.0f;
	float	 m_fSpecularHighlight = 0.0f;
	float	 m_fMetallic = 0.0f;
	float	 m_fGlossyReflection = 0.0f;

	UINT m_nType = 0x00;
};

struct MATERIALSLOADINFO
{
	int m_nMaterials = 0;
	MATERIALLOADINFO	*m_pMaterials = NULL;
};

class CMaterialColors
{
public:
	CMaterialColors() { }
	CMaterialColors(MATERIALLOADINFO *pMaterialInfo);
	virtual ~CMaterialColors() { }

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4 m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	XMFLOAT4 m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); //(r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
};

struct SRVROOTARGUMENTINFO
{
	UINT	 m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dSrvGpuDescriptorHandle;
};

class CTexture
{
public:
	CTexture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE2D, int nSamplers = 0);
	virtual ~CTexture();

private:
	int m_nReferences = 0;

	UINT  m_nTextureType = RESOURCE_TEXTURE2D;
	int m_nTextures = 0;
	ID3D12Resource **m_ppd3dTextures = NULL;
	ID3D12Resource **m_ppd3dTextureUploadBuffers;

	SRVROOTARGUMENTINFO *m_pRootArgumentInfos = NULL;

	int m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE *m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex);

	int GetTextures() { return(m_nTextures); }
	ID3D12Resource *GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	UINT GetTextureType() { return(m_nTextureType); }

	void ReleaseUploadBuffers();
};

class CMaterial
{
private:
	int m_nReferences = 0;

public:
	CMaterial();
	virtual ~CMaterial();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	CShader* m_pShader = NULL;
	CTexture* m_pTexture = NULL; 
	CMaterialColors* m_pMaterialColors = NULL;

	void SetMaterialColors(CMaterialColors *pMaterialColors);
	void SetShader(CShader *pShader);
	void SetIlluminatedShader() { SetShader(m_pIlluminatedShader); }
	void SetTexture(CTexture * pTexture);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList);

public:
	static CShader *m_pIlluminatedShader;
	static void CMaterial::PrepareShaders(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

class CGameObject
{
private: 
	int m_nReferences = 0;

public:
	void AddRef();
	void Release();

public:
	CGameObject(int nMeshes=1);
    virtual ~CGameObject();

public:
	char m_pstrFrameName[64];
	CMesh* m_pMesh = NULL;
	CMesh** m_ppMeshes ;
	int m_nMeshes;
	int m_nMaterials = 0;
	CMaterial **m_ppMaterials = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;


	XMFLOAT4X4 m_xmf4x4Transform;
	XMFLOAT4X4 m_xmf4x4World;

	CGameObject *m_pParent = NULL;
	CGameObject *m_pChild = NULL;
	CGameObject *m_pSibling = NULL;

	ID3D12Resource	*m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO	*m_pcbMappedGameObjects = NULL;

	void SetMesh(CMesh *pMesh);
	void SetMesh(int nIndex, CMesh *pMesh); //
	void SetShader(CShader *pShader);
	void SetShader(int nMaterial, CShader *pShader);
	void SetMaterial(int nMaterial, CMaterial *pMaterial);
	void SetTexture(CTexture* tex);

	void SetChild(CGameObject* pChild, bool bReferenceUpdate = false);

	virtual void BuildMaterials(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	virtual void OnInitialize() { }
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent=NULL);

	virtual void OnPrepareRender() { }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);
	virtual void Animates(float fTimeElapsed, CCamera* pCamera) {}
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, CMaterial *pMaterial);

	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetLook(float x, float y, float z);
	void SetLook(XMFLOAT3 xmf3LookVector);
	void SetRight(XMFLOAT3 xmf3RightVector);
	void SetRight(float x, float y, float z);

	void SetScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4 *pxmf4Quaternion);

	CGameObject *GetParent() { return m_pParent; }
	void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent=NULL);
	CGameObject *FindFrame(char *pstrFrameName);

	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0); }
public:
	static MATERIALSLOADINFO *LoadMaterialsInfoFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FILE *pInFile);
	static CMeshLoadInfo *LoadMeshInfoFromFile(FILE *pInFile);
	static CGameObject *LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, FILE *pInFile);
	static CGameObject *LoadGeometryFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName);

	static void PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent);
};

class CApacheObject : public CGameObject
{
public:
	CApacheObject();
	virtual ~CApacheObject();

private:
	CGameObject	*m_pMainRotorFrame = NULL;
	CGameObject	*m_pTailRotorFrame = NULL;
	
	XMFLOAT3 m_RandomPos;

public:
	virtual void OnInitialize();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent = NULL);
	void SetRandomPosition(XMFLOAT3);
	void MoveApachObject();
	XMFLOAT3 GetRandomPos(); 

};

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();

private:
	int m_nWidth;
	int m_nLength;
	XMFLOAT3 m_xmf3Scale;
	CHeightMapImage *m_pHeightMapImage;
	bool WIREFRAMEmode = false;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	void SetTerrainMode() 
	{
		if(!WIREFRAMEmode)
		WIREFRAMEmode = true; 
		else
		WIREFRAMEmode = false;

	}
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

};


class CSeaWater : public CGameObject
{
public:
	CSeaWater(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, float Width, float  Length, float xmf3Scale);
	virtual ~CSeaWater();

private:
	float m_nWidth;
	float m_nLength;
	float m_xmf3Scale;
};


struct VS_VB_BILLBOARD_INSTANCE
{
	XMFLOAT3              m_xmf3Position; 
	XMFLOAT2              m_xmf4BillboardInfo; 
};

class CBillboard : public CGameObject
{
public:
	CBillboard(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth, float fPosX, float fPosY, float fPosZ);
	virtual ~CBillboard();

	virtual void Animates(float fTimeElapsed, CCamera* pCamera);
	void SetLookAt(XMFLOAT3& xmf3TargetCamera);

};


class CStartView : public CGameObject
{
public:
	CStartView(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CStartView();

};

struct CB_TESSELLATION_INFO 
{ 
	float m_fTessellationFector;
	float m_InsideTessFactor; 
};

class CTessellationFector 
{
protected:
	ID3D12Resource* m_pd3dcbTessellationFector = NULL;
	CB_TESSELLATION_INFO* m_pcbMappedFector = NULL;

public:
	float Fector;
	float InsideFactor; 
	CTessellationFector();
	~CTessellationFector();

    void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    void ReleaseShaderVariables();
    void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
};

