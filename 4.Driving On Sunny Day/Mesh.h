//------------------------------------------------------- ----------------------
// File: Mesh.h
//-----------------------------------------------------------------------------
#pragma once
class CDiffused2TexturedVertex 
{
public: 
	XMFLOAT3				m_xmf3Position;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT2				m_xmf2TexCoord0;
	XMFLOAT2				m_xmf2TexCoord1;

public:
	CDiffused2TexturedVertex()
	{ 
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		m_xmf2TexCoord0 = m_xmf2TexCoord1 = XMFLOAT2(0.0f, 0.0f); 
	}

	CDiffused2TexturedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse, XMFLOAT2 xmf2TexCoord0, XMFLOAT2 xmf2TexCoord1)
	{ 
		m_xmf3Position = XMFLOAT3(x, y, z); 
		m_xmf4Diffuse = xmf4Diffuse;
		m_xmf2TexCoord0 = xmf2TexCoord0;
		m_xmf2TexCoord1 = xmf2TexCoord1;
	}

	CDiffused2TexturedVertex
	(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), 
		XMFLOAT2 xmf2TexCoord0 = XMFLOAT2(0.0f, 0.0f), XMFLOAT2 xmf2TexCoord1 = XMFLOAT2(0.0f, 0.0f))
	{ 
		m_xmf3Position = xmf3Position; 
		m_xmf4Diffuse = xmf4Diffuse;
		m_xmf2TexCoord0 = xmf2TexCoord0;
		m_xmf2TexCoord1 = xmf2TexCoord1;
	}
	~CDiffused2TexturedVertex() { }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CMesh
{
public:
	CMesh() { }
    virtual ~CMesh() { }

private:
	int	                        m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	virtual void ReleaseUploadBuffers() { }
protected:

	ID3D12Resource*             m_pd3dVertexBuffer = NULL; 
	ID3D12Resource*             m_pd3dVertexUploadBuffer = NULL;
					            
	ID3D12Resource*             m_pd3dIndexBuffer = NULL;
	ID3D12Resource*             m_pd3dIndexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW	m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		m_d3dIndexBufferView;
								
	D3D12_PRIMITIVE_TOPOLOGY	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	UINT	                    m_nSlot = 0;
	UINT	                    m_nVertices = 0;
	UINT	                    m_nOffset = 0;
	UINT	                    m_nType = 0;
	UINT	                    m_nStride = 0;
			                    
	UINT	                    m_nIndices = 0;
	UINT	                    m_nStartIndex = 0;
	int	                        m_nBaseVertex = 0;

public:
	UINT GetType() { return(m_nType); }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet) { }
	//virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet, UINT nInstances = 1) {}

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define VERTEXT_POSITION	0x01
#define VERTEXT_COLOR		0x02
#define VERTEXT_NORMAL		0x04
#define VERTEXT_TEXCOORD  0x08 


class CMeshLoadInfo
{
public:
	CMeshLoadInfo() { }
	~CMeshLoadInfo();

public:
	char               m_pstrMeshName[256] = { 0 };

	UINT               m_nType = 0x00;

	XMFLOAT3           m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3           m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	int	               m_nVertices = 0;
	XMFLOAT3           *m_pxmf3Positions = NULL;
	XMFLOAT4           *m_pxmf4Colors = NULL;
	XMFLOAT3           *m_pxmf3Normals = NULL;
	XMFLOAT2           *m_pxmf2Textures = NULL;  //텍스처UV주가

	int	               m_nIndices = 0;
	UINT               *m_pnIndices = NULL;
		 
	int	               m_nSubMeshes = 0;
	int	               *m_pnSubSetIndices = NULL;
	UINT               **m_ppnSubSetIndices = NULL;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CMeshFromFile : public CMesh
{
public:
	CMeshFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CMeshLoadInfo *pMeshInfo);
	virtual ~CMeshFromFile();

public:
	virtual void ReleaseUploadBuffers();

protected:
	ID3D12Resource				*m_pd3dPositionBuffer = NULL;
	ID3D12Resource				*m_pd3dPositionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dPositionBufferView;

	int							m_nSubMeshes = 0;
	int							*m_pnSubSetIndices = NULL;

	ID3D12Resource				**m_ppd3dSubSetIndexBuffers = NULL;
	ID3D12Resource				**m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW		*m_pd3dSubSetIndexBufferViews = NULL;

public:
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CMeshIlluminatedFromFile : public CMeshFromFile
{
public:
	CMeshIlluminatedFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CMeshLoadInfo *pMeshInfo);
	virtual ~CMeshIlluminatedFromFile();

	virtual void ReleaseUploadBuffers();

protected:
	ID3D12Resource				*m_pd3dNormalBuffer = NULL;
	ID3D12Resource				*m_pd3dNormalUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dNormalBufferView;

	ID3D12Resource				*m_pd3dTextureBuffer = NULL;
	ID3D12Resource				*m_pd3dTextureUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dTextureBufferView;

public:
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CHeightMapImage
{
private:
	BYTE						*m_pHeightMapPixels;

	int							m_nWidth;
	int							m_nLength;
	XMFLOAT3					m_xmf3Scale;

public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);

	float GetHeight(float x, float z, bool bReverseQuad = false);
	XMFLOAT3 GetHeightMapNormal(int x, int z);
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	BYTE *GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

class CHeightMapGridMesh : public CMesh
{
protected:
	int						m_nWidth;
	int						m_nLength;
	XMFLOAT3				m_xmf3Scale;

public:
	CHeightMapGridMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void *pContext = NULL);
	virtual ~CHeightMapGridMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	virtual float OnGetHeight(int x, int z, void *pContext);
	virtual XMFLOAT4 OnGetColor(int x, int z, void *pContext);

public:
	virtual void ReleaseUploadBuffers();
	//[질문]subset때문에 지형이 안 만들어 졌음... 뭐지? 
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);

};

class CTexturedVertex 
{
public:
	XMFLOAT3				m_xmf3Position;
	XMFLOAT2				m_xmf2TexCoord;
	//선언하는 순서도 아주 중요하다 !! 
	//선언하는 순서와 셰이더의 변수와 순서가 맞지 않으면 제대로 렌더링이 안된다.

public:
	CTexturedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); }
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf2TexCoord = xmf2TexCoord; }
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf2TexCoord = xmf2TexCoord; }
	~CTexturedVertex() { }
};

typedef struct SGSTexturedVertex
{
	CTexturedVertex      m_texturedVertex; 
	XMFLOAT2             m_xmf2Size;
}GSVERTEXT;

class CWaterSquare : public CMesh
{
public:
	CWaterSquare(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth , float fHeight , float fDepth );
	virtual ~CWaterSquare();
	virtual void ReleaseUploadBuffers();
	//[질문]subset때문에 지형이 안 만들어 졌음... 뭐지? 
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);
};


class CBillboardRectMesh : public CMesh
{
public:
	CBillboardRectMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		float fWidth, float fHeight, float fDepth, float fxPosition, float fyPosition, float fzPosition);
	virtual ~CBillboardRectMesh();
	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);
};

class CStartScreenRectMesh : public CMesh
{
public:
	CStartScreenRectMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		float fWidth, float fHeight, float fDepth, float fxPosition, float fyPosition, float fzPosition);
	virtual ~CStartScreenRectMesh();
	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);
};

class CRawFormatImage
{
protected:
	BYTE* m_pRawImagePixels = NULL;

	int							m_nWidth;
	int							m_nLength;

public:
	CRawFormatImage(const LPCTSTR pFileName, int nWidth, int nLength, bool bFlipY = false);
	~CRawFormatImage(void);

	BYTE GetRawImagePixel(int x, int z) { return(m_pRawImagePixels[x + (z * m_nWidth)]); }
	void SetRawImagePixel(int x, int z, BYTE nPixel) { m_pRawImagePixels[x + (z * m_nWidth)] = nPixel; }

	BYTE* GetRawImagePixels() { return(m_pRawImagePixels); }

	int GetRawImageWidth() { return(m_nWidth); }
	int GetRawImageLength() { return(m_nLength); }
};