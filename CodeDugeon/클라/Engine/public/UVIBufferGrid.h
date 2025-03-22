#pragma once
#include "UVIBuffer.h"

BEGIN(Engine)
/*
@ Date: 2024-02-08, Writer:이성현
@ Explain
- 툴의 기본 그리드 메쉬를 위한 VI버퍼
*/

const _uint GRID_SIZE = 300;
const _uint HALF_GRID_SIZE = GRID_SIZE / 2;

class  UVIBufferGrid : public UVIBuffer {
public:
	UVIBufferGrid(CSHPTRREF <UDevice> _spDevice, const VIBUFFERTYPE _eBufferType);
	UVIBufferGrid(const UVIBufferGrid& _rhs);
	DESTRUCTOR(UVIBufferGrid)
public:
	CLONE_THIS(UVIBufferGrid)
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
private:
};

END
