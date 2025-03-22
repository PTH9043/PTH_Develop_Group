#pragma once
#include "UBase.h"

BEGIN(Engine)
class UDevice;
class UGpuCommand;

class USwapChain final : public UBase {
public:
	USwapChain();
	NO_COPY(USwapChain)
		DESTRUCTOR(USwapChain)
public:
	const ComPtr<DxGISwapChain>& GetSwapChain() const { return m_cpSwapChain; }
	const _uint GetBackBufferIndex() const { return m_iBackBufferIndex; }
	const HANDLE& GewtSwapChainWaitableObject() const { return m_hSwapChainWaitableObject; }
	const DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() const { return m_stDXSwapChainDesc; }
public:
	virtual void Free() override;
	HRESULT NativeConstruct(const GRAPHICDESC& _stGraphicDesc, const _uint _iMssa4xQuality, const _bool _isMssa4x,
		CSHPTRREF<UDevice> _spDevice, CSHPTRREF<UGpuCommand> _spGpuCmd);
	HRESULT Present();
	void SwapChainIndx() { m_iBackBufferIndex = (m_iBackBufferIndex + 1) % SWAP_CHAIN_END; }
private:
	ComPtr<DxGISwapChain>		m_cpSwapChain;
	_uint											m_iBackBufferIndex;
	HANDLE										m_hSwapChainWaitableObject;
	DXGI_SWAP_CHAIN_DESC1	m_stDXSwapChainDesc;
};

END
