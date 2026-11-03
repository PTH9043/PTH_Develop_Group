#pragma once
#include"CActor.h"

class CRenderer;

class CPawn abstract : public CActor {
protected:
	explicit CPawn() = default;
	explicit CPawn(const CPawn& _rhs) :CActor(_rhs) {}
	virtual ~CPawn() = default;

public:
	virtual shared_ptr<CActor> Clone(const VOIDDATAS& _tDatas) override { return nullptr; }
	virtual void Free() override;

protected:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;

public:
	virtual HRESULT CreateComponent() override;

public:
	virtual HRESULT Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(const HDC& _hDC) override;

public:
	const shared_ptr<CRenderer>& Get_Renderer() const { return m_pRenderer; }

private:

	shared_ptr<CRenderer>			m_pRenderer{ nullptr };
};

