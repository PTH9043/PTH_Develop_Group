#pragma once
#include "CBuffer.h"

class CCubeBuffer : public CBuffer{
protected:
	explicit CCubeBuffer() = default;
	explicit CCubeBuffer(const CCubeBuffer& _rhs);
	virtual ~CCubeBuffer() = default;
public:
	virtual shared_ptr<CComponent> Clone(const VOIDDATAS& _tDatas);
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
protected:
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;

};

