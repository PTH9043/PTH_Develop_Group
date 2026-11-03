#pragma once
#include "CBuffer.h"

class CFloorBuffer : public CBuffer {
public:
	typedef struct tagFloorBufferDesc {

		_float fWidth{ 0.f }, fDepth{ 0.f };
		_int iSubjects{ 0 };
	}FLOORBUFFERDESC;
protected:
	explicit CFloorBuffer() = default;
	explicit CFloorBuffer(const CFloorBuffer& _rhs);
	virtual ~CFloorBuffer() = default;
public:
	virtual shared_ptr<CComponent> Clone(const VOIDDATAS& _tDatas);
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
protected:
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;
	HRESULT MakeFloorBuffer(const _float& _fWidth, const _float& _fDepth, const _int& _iSubRects);
};

