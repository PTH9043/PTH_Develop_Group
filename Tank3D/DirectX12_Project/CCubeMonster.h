#pragma once
#include "CShooterMonster.h"

class CCubeMonster : public CShooterMonster {

protected:
	explicit CCubeMonster() : CShooterMonster() {};
	CCubeMonster(const CCubeMonster& _rhs) :CShooterMonster(_rhs) {}
	virtual ~CCubeMonster() = default;
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
	virtual void CreateMonster(const MONSTERSHOOTERDESC& _tMonsterDesc) override;
	// Move
	virtual void ControlMonster(const _double& _dTimeDelta) override;

private:
	// CubeBuffer
	shared_ptr<CCubeBuffer>			m_pCubeBuffer = nullptr;
};

