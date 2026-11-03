#pragma once
#include "CCharacter.h"

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
class CCristal;
class CGameScene;

enum MONTYPE {

	MONSHOOTER,  MONJUST, MONBOSE,  MON_END
};

class CMonster abstract : public CCharacter {
public:
	typedef struct tagMonsterDecs : public CHARDESC {
		_uint iHp{ 0 };
	}MONSTERDESC;
protected:
	explicit CMonster(const MONTYPE _eType) : m_eMonType(_eType), CCharacter(CHAR_MONSTER) {}
	CMonster(const CMonster& _rhs) :CCharacter(_rhs) {}
	virtual ~CMonster() = default;

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
	// Move
	virtual void ControlMonster(const _double& _dTimeDelta);
	// Collision To Cristal
	_bool IsCollisionToCristal();
	// Damaged
	virtual void Damaged(const shared_ptr<CCharacter>& _pChar) override;
public:
	void Set_Target(const shared_ptr<CTank>& _pTank) {	this->m_pTarget = _pTank;}
	void Set_Cristal(const shared_ptr<CCristal>& _pCristal) { this->m_pCristal = _pCristal; }
	void Set_MoveSpeed(const _float _fMoveSpeed) { this->m_fMonsterMoveSpeed = _fMoveSpeed; }
	void Set_RotateSpeed(const _float _fRotateSpeed) { this->m_fMonsterRotateSpeed = _fRotateSpeed; }

	void Set_GameScene(const shared_ptr<CGameScene>& _pGameScene) { this->m_pGameScene = _pGameScene; }
protected:
	void Set_CollisionCristal(const _bool _bCristal) { this->m_bCollisionCristal = _bCristal; }
	void Set_Health(const _int _iHealth) { this->m_iHealth = _iHealth; }

	const weak_ptr<CTank>& Get_Target() const { return m_pTarget; }
	const weak_ptr<CCristal>& GetCrilstal() const { return m_pCristal; }
	const _float Get_MoveSpeed() const { return m_fMonsterMoveSpeed; }
	const _float Get_RotateSpeed() const { return m_fMonsterRotateSpeed; }
	const _bool IsCollisionCristal() const { return m_bCollisionCristal; }
private:
	// Game Scene
	weak_ptr<CGameScene>			m_pGameScene{};
	// Target 
	weak_ptr< CTank>						m_pTarget{};
	weak_ptr<CCristal>						m_pCristal{};
	// MonType
	MONTYPE										m_eMonType{};
	// Tank Value
	_float												m_fMonsterMoveSpeed = 0.f;
	_float												m_fMonsterRotateSpeed = 0.f;

	_bool												m_bCollisionCristal = false;
	_int													m_iHealth{ 0 };
};

