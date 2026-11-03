#pragma once
#include "CActor.h"

class CCubeBuffer;
class CRenderer;

class CEmptyObject : public CActor {
protected:
	explicit CEmptyObject(){}
	CEmptyObject(const CEmptyObject& _rhs) : CActor(_rhs) {}
	virtual ~CEmptyObject() = default;

public:
	virtual shared_ptr<CActor> Clone(const VOIDDATAS& _tDatas) override;
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;

public:
	virtual HRESULT CreateComponent() override;

public:
	virtual HRESULT Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(const HDC& _hDC) override;

private:
#ifdef _DEBUG
	shared_ptr<CCubeBuffer>		m_pCubeBuffer{ nullptr };
	shared_ptr<CRenderer>			m_pRenderer{ nullptr };
#endif
};

