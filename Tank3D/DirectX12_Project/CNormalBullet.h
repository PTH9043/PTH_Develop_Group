#pragma once
#include "CBullet.h"

class CNormalBullet : public CBullet {
protected:
	explicit CNormalBullet() : CBullet(BUL_NORMAL) {}
	CNormalBullet(const CNormalBullet& _rhs) :CBullet(_rhs) {}
	virtual ~CNormalBullet() = default;

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
	// Damaged 
	virtual _float GiveDamage() override;
};

