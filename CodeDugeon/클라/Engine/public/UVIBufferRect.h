#pragma once
#include "UVIBuffer.h"

BEGIN(Engine)

class  UVIBufferRect : public UVIBuffer{
public:
	UVIBufferRect(CSHPTRREF <UDevice> _spDevice, 	const VIBUFFERTYPE _eBufferType);
	UVIBufferRect(const UVIBufferRect& _rhs);
	DESTRUCTOR(UVIBufferRect)
public:
	CLONE_THIS(UVIBufferRect)
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
private:
};

END