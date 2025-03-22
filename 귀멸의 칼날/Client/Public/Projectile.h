#pragma once
#include "Pawn.h"

BEGIN(Engine)
class CModel;
class CTransform;
class CNavigation;
class CEffect;
class CCollider;
END

BEGIN(Client)

// 투사체 공격 클래스
class CProjectile : public CPawn
{
public:
	// 투사체 정보 구조체
	typedef struct tagProjectileDesc
	{
		// 주인 객체
		CGameObject*	pOwner = nullptr;
		// 타겟 객체
		CGameObject*	pTarget = nullptr;
		// 네비게이션 객체
		CNavigation*	pNavigation = nullptr;
		// 타겟의 콜라이더와 충돌했는지 판단하기 위해 상대방의 콜라이더를 받아온다.
		CCollider*		pTargetCollider = nullptr;
		// 객체가 살아있을 총 시간
		_float			fLifeTime = 0.f;
		// 데미지
		_float			fDamage = 0.f;
		// 날아갈 속도
		_float			fSpeed = 0.f;
		// 움직일 방향 값 ( 노멀라이즈 된 값 )
		_float3			vMoveDirection = _float3(0.f, 0.f, 0.f);
		// 처음 생성될 위치
		_float3			vCreatePos = _float3(0.f, 0.f, 0.f);
	}PROJECTILEDESC;

protected:
	explicit CProjectile(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CProjectile(const CProjectile& _rhs);
	virtual ~CProjectile() = default;

public:
	// 투사체가 가진 콜라이더 객체를 받아온다.
	CCollider* Get_Collider() { return m_pColliderCom; }

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
	virtual HRESULT Delete_Self(void);
	// 현재 객체를 움직이는 함수
	virtual void Move(const _double& _dTimeDelta);
	// 타이머를 실행한다.
	virtual void Running_Timer(const _double& _dTimeDelta);
	// 충돌 됐는지 체크한다.
	virtual void Check_Collision();

protected:
	// 콜라이더 객체
	CCollider*			m_pColliderCom = nullptr;
	// 투사체에 붙어서 다닐 이펙트 객체
	CGameObject*		m_pMainEffect = nullptr;
	// 투사체 정보
	PROJECTILEDESC		m_tProjectileDesc;
	// 객체의 라이프타임 타이머
	_double				m_dTimeAcc = 0.0;	

public:
	static CProjectile*		Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void Free(void) override;
};
END