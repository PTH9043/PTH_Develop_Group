#pragma once
#include "UVIBuffer.h"

BEGIN(Engine)

class  UVIBufferPlane : public UVIBuffer {
public:
	UVIBufferPlane(CSHPTRREF <UDevice> _spDevice, const VIBUFFERTYPE _eBufferType);
	UVIBufferPlane(const UVIBufferPlane& _rhs);
	DESTRUCTOR(UVIBufferPlane)
public:
	CLONE_THIS(UVIBufferPlane)
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
private:
};

END