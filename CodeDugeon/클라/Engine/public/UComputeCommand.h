#pragma once
#include "UCommand.h"

BEGIN(Engine)
class UDevice;

class UComputeCommand : public UCommand {
public:
	UComputeCommand();
	NO_COPY(UComputeCommand)
	DESTRUCTOR(UComputeCommand)
public:
	virtual void Free() override;
	virtual HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, const ComPtr<Dx12CommandQueue>& _cpCommandQueue) override;
	virtual void WaitForSynchronization() override;
	// Begin Bind
	virtual void BindRootSignature(CSHPTRREF<URootSignature> _spRootSignature) override;
	virtual void Clear() override;
	/* Variable */
private:
	ComPtr<Dx12CommandAllocator>		m_cpCommandAllocator;
};

END