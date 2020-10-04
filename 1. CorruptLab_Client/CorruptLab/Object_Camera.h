#pragma once

#define ASPECT_RATIO				(float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

#define FIRST_PERSON_CAMERA			0x01
#define SPACESHIP_CAMERA			0x02
#define THIRD_PERSON_CAMERA			0x03

struct VS_CB_EYE_CAMERA_PROJECTION
{
	XMFLOAT4X4						m_xmf4x4View;
	XMFLOAT4X4						m_xmf4x4Projection;
	XMFLOAT4X4						m_xmf4x4InverseView;
	XMFLOAT4X4						m_xmf4x4InverseProjection;
	XMFLOAT4						m_xmf3Position;
	XMFLOAT3						m_xmf3Look;
};
class CPlayer;

class CCamera
{
public:
	CCamera();
	~CCamera() {}

protected:

	DWORD			            	m_nMode;
	XMFLOAT3		            	m_xmf3Position;
	XMFLOAT3		            	m_xmf3Right;
	XMFLOAT3		            	m_xmf3Up;
	XMFLOAT3		            	m_xmf3Look;

	float                       	m_fPitch;
	float                       	m_fRoll;
	float                       	m_fYaw;


	XMFLOAT3		            	m_xmf3LookAtWorld;
	XMFLOAT3		            	m_xmf3Offset;
	float                       	m_fTimeLag;


	XMFLOAT4X4		            	m_xmf4x4View;
	XMFLOAT4X4		            	m_xmf4x4Projection;
	XMFLOAT4X4                      m_xmf4x4Ortho;

	XMFLOAT4X4						m_xmf4x4InverseView;
	XMFLOAT4X4						m_xmf4x4InverseProjection;

	D3D12_VIEWPORT	            	m_d3dViewport;
	D3D12_RECT		            	m_d3dScissorRect;


	ID3D12Resource                * m_pd3dcbvProjectionCamera = NULL;
	VS_CB_EYE_CAMERA_PROJECTION   * m_pcbMappedProjectionCamera = NULL;

	float                          m_fSunCameraPosition[3]; 
	
public:

	BoundingFrustum					m_boundingFrustum;

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	XMFLOAT3& GetPosition() { return(m_xmf3Position); }

	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }
	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }

	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }
	XMFLOAT4X4 GetOrthoMatrix() { return m_xmf4x4Ortho; }
	D3D12_VIEWPORT GetViewport() { return(m_d3dViewport); }
	D3D12_RECT GetScissorRect() { return(m_d3dScissorRect); }

	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void RegenerateViewMatrix();

	// 투영변환행렬 직교, 원근 -----------------------------------------------------------
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	void GenerateOrthoLHMatrix(float fWidth, float fHeight, float fNearPlaneDistance, float fFarPlaneDistance);
	
	virtual void Move(const XMFLOAT3& xmf3Shift) {}
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed) { }
	virtual void Update(CCamera* Player) {}
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt) { }

	void SetShadowCameraPosition(float ShadowPosition_x, float ShadowPosition_y, float ShadowPosition_z) 
	{
		m_fSunCameraPosition[0] = ShadowPosition_x;
		m_fSunCameraPosition[1] = ShadowPosition_y; 
		m_fSunCameraPosition[2] = ShadowPosition_z;
	}
	
	float* GetShadowCameraPosition() { return m_fSunCameraPosition; }

	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList,
		int RootParameterIndex = ROOT_PARAMETER_CAMERA);

	DWORD GetMode() { return(m_nMode); }


};

class CPlayerCamera : public CCamera
{
public:

	CPlayer			              * m_pPlayer = NULL;
					               
public:

	void SetMode(DWORD nMode) { m_nMode = nMode; }

	CPlayerCamera() {}
	CPlayerCamera(CPlayerCamera*pCamera);
	virtual ~CPlayerCamera();


	void SetOffset(XMFLOAT3 xmfOffset) { m_xmf3Offset = xmfOffset; }
	XMFLOAT3& GetOffset() { return m_xmf3Offset; }

	//virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	//virtual void ReleaseShaderVariables();
	//virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, int RootParameterIndex = ROOT_PARAMETER_CAMERA);


	void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }
	CPlayer *GetPlayer() { return(m_pPlayer); }


	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }

	virtual void Move(const XMFLOAT3& xmf3Shift)
	{
		m_xmf3Position.x += xmf3Shift.x; 
		m_xmf3Position.y += xmf3Shift.y;
		m_xmf3Position.z += xmf3Shift.z;
	}

};

class CSpaceShipCamera : public CPlayerCamera
{
public:
	CSpaceShipCamera(CPlayerCamera*pCamera);
	virtual ~CSpaceShipCamera() { }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CFirstPersonCamera : public CPlayerCamera
{
public:
	CFirstPersonCamera(CPlayerCamera*pCamera);
	virtual ~CFirstPersonCamera() { }

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CThirdPersonCamera : public CPlayerCamera
{
public:
	CThirdPersonCamera(CPlayerCamera *pCamera);
	virtual ~CThirdPersonCamera() { }

	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& vLookAt);
	//virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CSunCamera : public CCamera
{
public:
	CSunCamera(XMFLOAT3 pos, XMFLOAT3 Look, XMFLOAT3 Right, XMFLOAT3 Up);
	virtual ~CSunCamera() {}

public:
	virtual void Update(CCamera* Player);
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

};