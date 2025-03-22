#pragma once

#include "CBase.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)
public:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;
public:
	HRESULT Ready_Graphic_Device(const HWND& hWnd, const GRAPHICDESC::WINMODE& WinMode, 
		const _uint& iWinCX, const _uint& iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);

	HRESULT Clear_BackBuffer_View(const _float4& vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Set_TargetView(); 
	HRESULT Present();
private:
	/* 메모리 할당. (정점버퍼, 인덱스버퍼, 텍스쳐로드) */
	ID3D11Device*			m_pDevice = nullptr;
	/* 바인딩작업. 기능실행 (정점버퍼를 SetStreamSource(), SetIndices(), SetTransform(), SetTexture() */
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	IDXGISwapChain*			m_pSwapChain = nullptr;
	// ID3D11ShaderResourceView*
	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;
	// Blend State
	ID3D11BlendState* m_pBlendState = nullptr;
private:
	HRESULT Ready_SwapChain(const HWND& _hWnd, const GRAPHICDESC::WINMODE& eWinMode,
		 const  _uint& _iWinCX, const _uint& _iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(const _uint& _iWinCX, const _uint& _iWinCY);
	HRESULT Create_BlendState();
public:
	virtual void Free() override;
};

END