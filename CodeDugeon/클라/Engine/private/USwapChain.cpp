#include "EngineDefine.h"
#include "USwapChain.h"
#include "UDevice.h"
#include "UGpuCommand.h"

USwapChain::USwapChain() :
	m_cpSwapChain{ nullptr },
	m_iBackBufferIndex{ 0 },
	m_hSwapChainWaitableObject{ NULL },
	m_stDXSwapChainDesc{}
{
}

void USwapChain::Free()
{
}

HRESULT USwapChain::NativeConstruct(const GRAPHICDESC& _stGraphicDesc, const _uint _iMssa4xQuality, const _bool _isMssa4x,
	CSHPTRREF<UDevice> _spDevice, CSHPTRREF<UGpuCommand> _spGpuCmd)
{
	RETURN_CHECK(nullptr == _spDevice || nullptr == _spGpuCmd, E_FAIL);

	DXGI_SWAP_CHAIN_DESC1 sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = SWAP_CHAIN_END;
		sd.Width = _stGraphicDesc.iWinCX;
		sd.Height = _stGraphicDesc.iWinCY;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = _isMssa4x ? 4 : 1;
		sd.SampleDesc.Quality = _isMssa4x ? (_iMssa4xQuality - 1) : 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling = DXGI_SCALING_STRETCH;
		sd.Stereo = FALSE;
	}
	::memcpy(&m_stDXSwapChainDesc, &sd, sizeof(sd));
	ComPtr<IDXGISwapChain1> swapChain1 = NULL;
	ComPtr<ID3D12CommandQueue> pCmdQueue = _spGpuCmd->GetCmdQue();

	// Create Swap Chain For HWnd 
	RETURN_CHECK_DXOBJECT(_spDevice->GetFactory()->CreateSwapChainForHwnd(pCmdQueue.Get(), _stGraphicDesc.hWnd,
		&sd, NULL, NULL, &swapChain1), E_FAIL);

	// SwapChainÀÇ QueryInterface·Î ¸¸µë 
	RETURN_CHECK_DXOBJECT(swapChain1->QueryInterface(IID_PPV_ARGS(&m_cpSwapChain)), E_FAIL);

	// SWAP_CHAIN_END = 3
	m_cpSwapChain->SetMaximumFrameLatency(SWAP_CHAIN_END);
	m_hSwapChainWaitableObject = m_cpSwapChain->GetFrameLatencyWaitableObject();
	return S_OK;
}

HRESULT USwapChain::Present()
{
	m_cpSwapChain->Present(0, 0);
	return S_OK;
}
