#pragma once
#include "CPawn.h"

class COBBCollider;
class CAABBCollider;
class CFloor;
class CCollider;

enum CHARTYPE {
	CHAR_PLAYER, CHAR_MONSTER, CHAR_CRISTAL , CHAR_END
};

class CCharacter abstract : public CPawn {
public:
	typedef struct tagCharDesc {

		tagCharDesc() = default;

		_float3 vColor{};
		_float3 vBodyScale{};
	}CHARDESC;
protected:
	explicit CCharacter(const CHARTYPE _eCharType) : m_eCharType(_eCharType) {};
	CCharacter(const CCharacter& _rhs) :CPawn(_rhs) {}
	virtual ~CCharacter() = default;

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

	 _bool IsDamaged(const shared_ptr<CCollider>& _pAtkCollider, const shared_ptr<CCharacter>& _pChar);
	 _bool IsDamaged(const shared_ptr<CCollider>& _pFirstCheckCollider, const shared_ptr<CCollider>& _pAtkCollider,
		 const shared_ptr<CCharacter>& _pChar);
protected:
	// Damaged
	virtual void Damaged(const shared_ptr<CCharacter>& _pChar);
protected:
	void UpdateColliderScale();
public:
	void Set_Floor(const shared_ptr<CFloor>& _pFloor) { this->m_pFloor = _pFloor; }
public:
	const shared_ptr<COBBCollider>& Get_DamgedCollider() const { return m_pDamagedCollider; }
	const shared_ptr<CAABBCollider>& Get_FirstCheckCollider() const { return m_pFirstCheckCollider; }
public:

	const _float Get_Damage() const { return m_fDamage; }
	void Set_Damage(const _float _fDamge) { this->m_fDamage = _fDamge; }
protected:
	// Get Floor
	const weak_ptr<CFloor>& Get_Floor() const { return m_pFloor; }
private:
	// Floor
	weak_ptr<CFloor>						m_pFloor{ };
	// OBB Collider 
	shared_ptr<COBBCollider>		m_pDamagedCollider = nullptr;
	// AABB Collider 
	shared_ptr<CAABBCollider>		m_pFirstCheckCollider = nullptr;
	// Type
	CHARTYPE										m_eCharType{ CHAR_END };
	// Damage 
	_float												m_fDamage{ 1.f };
};

