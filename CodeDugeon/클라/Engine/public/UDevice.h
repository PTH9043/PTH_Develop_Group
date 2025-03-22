#pragma once
#include "UBase.h"

BEGIN(Engine)

class UDevice : public UBase{
public:
	UDevice();
	NO_COPY(UDevice)
	DESTRUCTOR(UDevice)
public:
	const ComPtr<Dx12Device>& GetDV() const { return m_cpDevice; }
	const ComPtr<DxGIFactory>& GetFactory() const { return m_cpFactory; }

	virtual void Free() override;
	HRESULT NativeConstruct();
private:
	ComPtr<Dx12Device>		m_cpDevice;
	ComPtr<DxGIFactory>		m_cpFactory;
	ComPtr<IDXGIAdapter1>	m_cpAdaptor;
#ifdef _DEBUG
	ComPtr<ID3D12Debug>		m_cpDebug;
#endif 
};

END

