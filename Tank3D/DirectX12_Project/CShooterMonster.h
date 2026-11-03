#pragma once
#include "CMonster.h"

class CCubeBuffer;
class COBBCollider;
class CControlHead;
class CPosin;
class CShooter;
class CCamera;
class CEmptyObject;
class CMainCamera;
class CCube;
class CFloor;
class CAABBCollider;
class CTank;

class CShooterMonster abstract : public CMonster {
public:
	typedef struct tagMonsterDesc : public MONSTERDESC {

		_float3 vFirePos{ 0.f, 0.f, 0.f };
		_float fPosinRotateSpeed = 0.f;
		_float fMoveSpeed = 0.f;
		_float fRotateSpeed = 0.f;

		_float fDamage = 0.f;
		_float fFireAbleTime = 0.f;
		_float fFireSpeed = 0.f;
		_float fLifeTime = 0.f;
		_float fReloadTime = 0.f;
		_uint iBulletNum = 0;

		_float3 vBulletColor{ 0.f, 0.f, 0.f };
		_float3 vBulletScale{ 0.f, 0.f, 0.f };
	}MONSTERSHOOTERDESC;
protected:
	explicit CShooterMonster() : CMonster(MONSHOOTER) {}
	CShooterMonster(const CShooterMonster& _rhs) :CMonster(_rhs) {}
	virtual ~CShooterMonster() = default;

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
	virtual void CreateMonster(const MONSTERSHOOTERDESC& _tMonsterDesc);
	// Move
	virtual void ControlMonster(const _double& _dTimeDelta) override;
protected:
	const shared_ptr<CShooter>& Get_Shooter() const { return m_pShooter; }
private:
	// TankShooter
	shared_ptr<CShooter>				m_pShooter = nullptr;
	// MonType
	MONTYPE										m_eMonType{};
};

