#pragma once
#include "UBase.h"

BEGIN(Engine)
class UDevice;
class UTableDescriptor;

class UShaderStructedBuffer final : public UBase {
public:
	UShaderStructedBuffer();
	NO_COPY(UShaderStructedBuffer)
	DESTRUCTOR(UShaderStructedBuffer)

public:
	const D3D12_RESOURCE_STATES& GetResourceState() const { return m_stD3DResourceState; }
	const ComPtr<Dx12Resource>& GetBufferResource() const { return m_cpBufferResource; }

public:
	// Free
	virtual void Free() override;
	HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, const _uint _iElementSize, const _uint _iElementCnt);
	HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, const size_t _iElementSize, const _uint _iElementCnt);

	void PushSRVData(CSHPTRREF<UTableDescriptor> _spTableDescriptor, const SRV_REGISTER& _eReg);
	void PushUAVData(CSHPTRREF<UTableDescriptor> _spTableDescriptor, const UAV_REGISTER& _eReg);
private:
	_uint 														m_iElementSize;
	_uint														m_iElementCount;
	D3D12_RESOURCE_STATES				m_stD3DResourceState;
	// Comptr
	ComPtr<Dx12Resource>					m_cpBufferResource;
	ComPtr<Dx12DescriptorHeap>		m_cpSrvDescriptorHeap;
	ComPtr<Dx12DescriptorHeap>		m_cpUavDescriptorHeap;
	// Heap Begin
	D3D12_CPU_DESCRIPTOR_HANDLE m_stD3DSrvHeapBegin;
	D3D12_CPU_DESCRIPTOR_HANDLE m_stD3DUavHeapBegin;
};

END