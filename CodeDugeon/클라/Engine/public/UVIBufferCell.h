#pragma once
#include "UVIBuffer.h"

BEGIN(Engine)

class  UVIBufferCell : public UVIBuffer {
public:
	UVIBufferCell(CSHPTRREF <UDevice> _spDevice);
	UVIBufferCell(const UVIBufferCell& _rhs);
	DESTRUCTOR(UVIBufferCell)
public:
	CLONE_THIS(UVIBufferCell)
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	HRESULT NativeConstruct(ARRAY<_float3, 3>& _Points);
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
private:
};

END