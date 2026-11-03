#pragma once
#include "CCubePawn.h"

class CCubeBuffer;
class COBBCollider;
class CCube;

class CPosin : public CCubePawn {
protected:
	explicit CPosin() = default;
	explicit CPosin(const CPosin& _rhs) :CCubePawn(_rhs) {}
	virtual ~CPosin() = default;
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

protected:
	// Is Over 
	_bool IsCheckOverAngleUp();
	// Is Over 
	_bool IsCheckOverAngleDown();

public:
	const _float Get_ShootAngle() const { return m_fShootAngle; }
private:
	 // Shoot Angle
	_float	m_fShootAngle = 0.f;
};

