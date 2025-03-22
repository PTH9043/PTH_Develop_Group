#pragma once
#include "UVIBuffer.h"

BEGIN(Engine)

class UVIBufferCube : public UVIBuffer{
public:
	UVIBufferCube(CSHPTRREF <UDevice> _spDevice, 	const VIBUFFERTYPE & _eCubeBufferType);
	UVIBufferCube(const UVIBufferCube& _rhs);
	DESTRUCTOR(UVIBufferCube)
public:
	CLONE_THIS(UVIBufferCube)
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _stDatas) override;
};

END
