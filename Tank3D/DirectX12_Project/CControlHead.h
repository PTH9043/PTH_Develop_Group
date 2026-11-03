#pragma once
#include "CCubePawn.h"

enum CONTROLHEAD {
	CH_RL, CH_UD, CH_ALL  , CH_END
};

class CControlHead : public CCubePawn {
protected:
	explicit CControlHead() = default;
	explicit CControlHead(const CControlHead& _rhs) :CCubePawn(_rhs) {}
	virtual ~CControlHead() = default;

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

public:

public:
	const _float Get_PosinRotateSpeed() const { return m_fPosinRotateSpeed; }
	void Set_PosinRotateSpeed(const _float _fPosinRotateSpeed) { this->m_fPosinRotateSpeed = _fPosinRotateSpeed; }
private:
	_float		m_fPosinRotateSpeed = 0.f;
	_float3	m_vRotate{ 0.f, 0.f, 0.f };
};

