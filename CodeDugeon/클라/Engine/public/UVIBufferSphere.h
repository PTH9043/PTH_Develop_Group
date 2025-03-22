#pragma once
#include "UVIBuffer.h"

BEGIN(Engine)

class UVIBufferSphere : public UVIBuffer{
public:
	UVIBufferSphere(CSHPTRREF <UDevice> _spDevice,	const VIBUFFERTYPE _eBufferType);
	UVIBufferSphere(const UVIBufferSphere& _rhs);
	DESTRUCTOR(UVIBufferSphere)
public:
	CLONE_THIS(UVIBufferSphere)
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
};

END
