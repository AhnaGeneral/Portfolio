#pragma once
#include "Mesh.h"

class CDiffused2TexturedVertex;

// CHeightMapImage-----------------------------------------------------------------------------------
class CHeightMapImage
{
private:
	BYTE                      * m_pHeightMapPixels = NULL;
	
	int							m_nWidth = 0;
	int							m_nLength = 0;
	XMFLOAT3					m_xmf3Scale = { 0.0f, 0.0f, 0.0f };

public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);

	float GetHeight(float x, float z, bool bReverseQuad = false);
	XMFLOAT3 GetHeightMapNormal(int x, int z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	BYTE* GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

struct HeightMapVertex
{
	XMFLOAT3						m_xmf3Position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4						m_xmf4Color = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMFLOAT3						m_xmf3Normal = { 0.0f, 0.0f, 0.0f };
	XMFLOAT2						m_xmf2TexCoord0 = { 0.0f, 0.0f };
	XMFLOAT2						m_xmf2TexCoord1 = { 0.0f, 0.0f };
};

class CHeightMapGridMesh : public CMesh
{
protected:
	int						m_nWidth = 0 ;
	int						m_nLength = 0;
	XMFLOAT3				m_xmf3Scale = { 0.0f, 0.0f, 0.0f };
	HeightMapVertex       * m_pVertices = NULL ; 
	XMFLOAT3			  * m_xmf3Positions = NULL ;

public:
	BoundingBox				m_boundingbox;

	CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth,
		int nLength, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void* pContext = NULL);
	virtual ~CHeightMapGridMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }
	HeightMapVertex GetVertex(int i) { return m_pVertices[i]; }

	virtual float OnGetHeight(int x, int z, void* pContext);
	virtual XMFLOAT4 OnGetColor(int x, int z, void* pContext);

public:
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);
};

