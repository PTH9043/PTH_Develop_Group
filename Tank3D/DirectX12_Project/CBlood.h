#pragma once
#include "CActor.h"

class CCube;

class CBlood : public CActor {
public:
	typedef struct tagBloodDesc {
		_uint iBloodCnt{ 0 };
	}BLOODDESC;
protected:
	explicit CBlood() = default;
	explicit CBlood(const CBlood& _rhs) :CActor(_rhs) {}
	virtual ~CBlood() = default;
public:
	virtual shared_ptr<CActor> Clone(const VOIDDATAS& _tDatas)override;
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas)override;

protected:
	virtual HRESULT CreateComponent() override;
public:
	virtual HRESULT Tick(const _double& _dTimeDelta)override;
	virtual HRESULT Late_Tick(const _double& _dTimeDelta)override;
	virtual HRESULT Render(const HDC& _hDC) override;

private:
	vector<shared_ptr<CCube>>	m_BloodList;
	vector<_float3>							m_BloodDirList;
	const _uint									BLOOD_CNT = 0;

	_double											m_dBloodTimer{ 0.f };
	_double											BLOOD_TIMER{ 10 };
};

