#pragma once
#include "Projectile.h"

BEGIN(Client)

// 투사체 공격 클래스
class CWindBall : public CProjectile
{
protected:
	explicit CWindBall(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CWindBall(const CWindBall& _rhs);
	virtual ~CWindBall() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(void) override;
	virtual HRESULT NativeConstruct_Clone(void* pArg);
	virtual void	Tick(const _double& _dTimeDelta) override;
	virtual void	Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(void) override;

protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable(void) override;
	// SetUp Components
	virtual HRESULT SetUp_Components(void) override;

protected:
	// 현재 객체를 삭제하는 함수
	virtual HRESULT Delete_Self(void) override;
	// 현재 객체를 움직이는 함수
	virtual void Move(const _double& _dTimeDelta) override;
	// 타이머를 실행한다.
	virtual void Running_Timer(const _double& _dTimeDelta) override;
	// 충돌 됐는지 체크한다.
	virtual void Check_Collision() override;

public:
	static CWindBall* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END