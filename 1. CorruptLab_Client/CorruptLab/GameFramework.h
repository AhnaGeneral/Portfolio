#pragma once

#include "Timer.h"
#include "Shader_MRT.h"
#include "Object_Player.h"
#include "Scene.h"
#include "LobbyScene.h"
#include "Shader_LightMRT.h"
#include "CShader_ShadowMRT.h"
#include "EndingScene.h"


class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateSwapChainRenderTargetViews();
	void CreateDepthStencilView();
	void CreateShadowDepthStencilView();

	void ChangeSwapChainState();

	void CreateOffScreenRenderTargetViews();
	void CreateLightRenderTargetViews();
	void CreateShadowRenderTargetViews();

	void BuildObjects();
	void ReleaseObjects();
	void ProcessInput();
	void ShadowMapRender();
	void FrameAdvanceStageIndoor();
	void FrameAdvanceStageOutdoor();
	void FrameAdvanceEnding();
	void FrameAdvanceLobby();
	UINT GetSceneState() { return m_nSceneState; }
	void TurnToIndoorState();
	void TurnToEnding();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:

	IDXGIFactory4                     * m_pdxgiFactory;
	IDXGISwapChain3                   * m_pdxgiSwapChain;
	ID3D12Device                      * m_pd3dDevice;

	ID3D12CommandAllocator            * m_pd3dCommandAllocator;
	ID3D12CommandQueue                * m_pd3dCommandQueue;
	ID3D12GraphicsCommandList         * m_pd3dCommandList;

	int								    m_nWndClientWidth;
	int								    m_nWndClientHeight;

	float								m_fSunTime;
	HINSTANCE						    m_hInstance;
	HWND							    m_hWnd;

	bool							    m_bMsaa4xEnable;
	UINT							    m_nMsaa4xQualityLevels;

	// 다중랜더타겟 [ 힙, 핸들, 버퍼 ]________________________________________________________________________
	ID3D12DescriptorHeap              * m_pd3dRtvDescriptorHeap;
	static const UINT				    m_nOffScreenRenderTargetBuffers = FINAL_MRT_COUNT;
	ID3D12Resource                    * m_ppd3dOffScreenRenderTargetBuffers[m_nOffScreenRenderTargetBuffers];
	D3D12_CPU_DESCRIPTOR_HANDLE		    m_pd3dOffScreenRenderTargetBufferCPUHandles[m_nOffScreenRenderTargetBuffers];
	
	ID3D12DescriptorHeap			  * m_pd3dLightDescriptorHeap;
	static const UINT				    m_nOffScreenLightBuffers = 1;
	ID3D12Resource                    * m_ppd3dLightMapRenderTargetBuffers[m_nOffScreenLightBuffers];
	D3D12_CPU_DESCRIPTOR_HANDLE		    m_pd3dOffScreenLightBufferCPUHandles[m_nOffScreenLightBuffers];

	ID3D12DescriptorHeap              * m_pd3dShadowDescriptorHeap;
	static const UINT				    m_nOffScreenShadowBuffers = 1;
	ID3D12Resource                    * m_ppd3dShadowRenderTargetBuffers[m_nOffScreenShadowBuffers];
	D3D12_CPU_DESCRIPTOR_HANDLE		    m_pd3dOffScreenShadowBufferCPUHandles[m_nOffScreenShadowBuffers];
    // _____________________________________________________________________________________________________

	UINT							    m_nSwapChainBufferIndex;
	static const UINT				    m_nSwapChainBuffers = 2;
	ID3D12Resource                    * m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];

	UINT							    m_nRtvDescriptorIncrementSize;
	D3D12_CPU_DESCRIPTOR_HANDLE		    m_pd3dRtvSwapChainBackBufferCPUHandles[m_nSwapChainBuffers];

	ID3D12Resource                    * m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap              * m_pd3dDsvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE		    m_d3dDsvDepthStencilBufferCPUHandle;

	ID3D12Resource                    * m_pd3dShadowDepthStencilBuffer;
	ID3D12DescriptorHeap              * m_pd3dShadowDsvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE		    m_d3dShadowDsvDepthStencilBufferCPUHandle;


	HANDLE							    m_hFenceEvent;
	ID3D12Fence                       * m_pd3dFence;
	UINT64							    m_nFenceValues[m_nSwapChainBuffers];

	CGameTimer						    m_GameTimer;

	UINT                                m_nSceneState;

     POINT							    m_ptOldCursorPos;

	 _TCHAR							    m_pszFrameRate[50];

public:
	CTexture                          * m_pFinalTexture;
	CTexture                          * m_pShadowMap;
	CTexture                          * m_pDepthTextue; 

private:
	CPostProcessingShader             * m_pPostProcessingShader;
	CLightTarget                      * m_pLightProcessingShader; 
	Shader_ShadowMRT                  * m_pShadowShader; 

	CScene                            * m_pScene[4];
	//UINT                                m_SceneItemReact;

	CPlayer                           * m_pPlayer ;
	CCamera                           * m_pCamera ;
};

