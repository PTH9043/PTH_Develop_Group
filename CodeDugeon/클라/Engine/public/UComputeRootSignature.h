#pragma once

#include "URootSignature.h"

BEGIN(Engine)

class UComputeRootSignature final : public URootSignature{
public:
	UComputeRootSignature();
	NO_COPY(UComputeRootSignature)
	DESTRUCTOR(UComputeRootSignature)
public:
	virtual void Free() override;
protected:
	virtual HRESULT CreateRootSignature(CSHPTRREF<UDevice> _spDevice);
};

END



