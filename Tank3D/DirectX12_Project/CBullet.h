#pragma once
#include"CCubePawn.h"

class COBBCollider;
class CAABBCollider;

enum BULLETTYPE {

	BUL_NORMAL, BUL_EXPLOSION, BUL_TARGET, BUL_END
};

class CShooter;
class CFloor;
class CCharacter;

class CBullet abstract : public CCubePawn {
public:
	typedef struct tagBulletDesc {
		_float	fLifeTime{ 0.f };
		_float	fDamage{ 0.f };
		_float fSpeed{ 0.f };
	}BULLETDESC;
protected:
	explicit CBullet(const BULLETTYPE _eBullType) : m_eBulletType(_eBullType){}
	CBullet(const CBullet& _rhs) :CCubePawn(_rhs) {}
	virtual ~CBullet() = default;

public:
	static vector<shared_ptr<CBullet>> CreateBulletVector(const BULLETTYPE _eBulletType,  BULLETDESC& _tBulletDesc, 
		const _uint _iBulletCnt, const _float3& _vBodyScale = _float3(0.1f, 0.1f, 0.1f), const _float3& _vColor = _float3(30.f, 255.f, 30.f));
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
	// Damaged 
	virtual _float GiveDamage() = 0;
	// Update FireDir
	void UpdateFireDir(const shared_ptr<CCharacter>& _pChar, const shared_ptr<CFloor>& _pFloor,
		const _fvector& _vFirePos, const _fvector& _vFireDir, const _float _fLifeTime, const _float _fDamge,
		const _float _fSpeed, const _float& _fShootAngle);
	// CheckTarget
	_bool IsCheckTargetHit(const shared_ptr<CCharacter>& _pChar);
protected:
	// Move Bullet
	void MoveBullet(const _double& _dTimeDelta);
	void Reset();
public:
	const BULLETTYPE& Get_BulletType() const { return m_eBulletType; }
	const _float& Get_LifeTime() const { return m_BulletDesc.fLifeTime; }
	const _float& Get_Damage() const { return m_BulletDesc.fDamage; }
	const _float& Get_BulletSpeed() const { return m_BulletDesc.fSpeed; }
protected:
	const _float3& Get_FireDir() const { return m_vFireDir; }
public:
	const weak_ptr<CCharacter>& Get_Owner() const { return m_pOwner; }

	void Set_Shooter(const shared_ptr<CShooter>& _pShooter) { this->m_pShooter = _pShooter; }
private:
	// OBBCollider
	shared_ptr<COBBCollider>	m_pHitCollider = nullptr;
	shared_ptr<CAABBCollider>	m_pFirstCheckCollider = nullptr;
	BULLETTYPE								m_eBulletType{ BUL_END };
	_float3										m_vFireDir{ 0.f, 0.f, 0.f };
	// Bullet Desc 
	BULLETDESC							m_BulletDesc{};
	// ´©Àû
	_float											m_fAccumulLifeTime{0.f};
	// Shoot Angle
	_float											m_fShootAngle{ 0.f };
	// Floor
	weak_ptr<CFloor>					m_pFloor{ };
	// Character
	weak_ptr<CCharacter>			m_pOwner{};
	// Shooter
	weak_ptr<CShooter>				m_pShooter{};
};

