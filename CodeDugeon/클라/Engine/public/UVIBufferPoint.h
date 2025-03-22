#pragma once

#include "UVIBuffer.h"

BEGIN(Engine)

class UVIBufferPoint : public UVIBuffer{
public:
	UVIBufferPoint(CSHPTRREF <UDevice> _spDevice);
	UVIBufferPoint(const UVIBufferPoint& _rhs);
	DESTRUCTOR(UVIBufferPoint)
public:
	CLONE_THIS(UVIBufferPoint)
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _convecDatas) override;
};

END
