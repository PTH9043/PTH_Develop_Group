#pragma once
#include "CBuffer.h"

class CCristalBuffer : public CBuffer {
protected:
	explicit CCristalBuffer() = default;
	explicit CCristalBuffer(const CCristalBuffer& _rhs);
	virtual ~CCristalBuffer() = default;
public:
	virtual shared_ptr<CComponent> Clone(const VOIDDATAS& _tDatas);
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
protected:
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;

};

