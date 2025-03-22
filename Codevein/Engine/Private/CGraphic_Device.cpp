#include "..\public\CGraphic_Device.h"

IMPLEMENT_SINGLETON(CGraphic_Device)

CGraphic_Device::CGraphic_Device()
	: m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
{



}

HRESULT CGraphic_Device::Ready_Graphic_Device(const HWND& hWnd, const GRAPHICDESC::WINMODE& WinMode,
	const _uint& iWinCX, const _uint& iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL			FreatureLV;

	/* 그래픽 장치를 초기화한다. */
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FreatureLV, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_SwapChain(hWnd, WinMode, iWinCX, iWinCY)))
		return E_FAIL;

	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilRenderTargetView(iWinCX, iWinCY)))
		return E_FAIL;

	if (FAILED(Create_BlendState()))
		return E_FAIL;

	/* 장치에 바인드해놓을 렌더타겟들과 뎁스스텐실뷰를 셋팅한다. */
	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilView);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)iWinCX;
	ViewPortDesc.Height = (_float)iWinCY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	*ppDeviceOut = m_pDevice;
	*ppDeviceContextOut = m_pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(const _float4& vClearColor)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	/* 백버퍼를 초기화하낟.  */
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&vClearColor);
	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Set_TargetView()
{
	/* 장치에 바인드해놓을 렌더타겟들과 뎁스스텐실뷰를 셋팅한다. */
	//m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilView);
	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	return m_pSwapChain->Present(0, 0);
}

HRESULT CGraphic_Device::Ready_SwapChain(const HWND& _hWnd, const GRAPHICDESC::WINMODE& eWinMode,
	const  _uint& _iWinCX, const _uint& _iWinCY)
{

	IDXGIDevice*			pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter*			pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory*			pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	SwapChain.BufferDesc.Width = _iWinCX;
	SwapChain.BufferDesc.Height = _iWinCY;
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;
	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 1;
	SwapChain.OutputWindow = _hWnd;
	SwapChain.Windowed = eWinMode;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
		return E_FAIL;

	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (nullptr == m_pDevice)
		return E_FAIL;


	ID3D11Texture2D*		pBackBufferTexture = nullptr;

	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilRenderTargetView(const _uint& _iWinCX, const _uint& _iWinCY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = _iWinCX;
	TextureDesc.Height = _iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}


HRESULT CGraphic_Device::Create_BlendState()
{
	//D3D11_BLEND_DESC bd;
	//ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));

	//bd.AlphaToCoverageEnable = false;
	//bd.IndependentBlendEnable = false;
	//bd.RenderTarget[0].BlendEnable = true;
	//bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//// A 연산 저장?
	//bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL ;

	//if (FAILED(m_pDevice->CreateBlendState(&bd, &m_pBlendState)))
	//	return E_FAIL;

	return S_OK;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
