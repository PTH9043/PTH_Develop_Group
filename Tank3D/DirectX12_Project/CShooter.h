#pragma once
#include"CCubePawn.h"

class CBullet;
enum BULLETTYPE;
class CFloor;
class CCharacter;

typedef vector<shared_ptr<CBullet>> BULLETLIST;
typedef list<shared_ptr<CBullet>> FIREBULLETLIST;
typedef unordered_map<BULLETTYPE, BULLETLIST> BULLETSAVER;

class CShooter : public CCubePawn {
public:
	typedef struct tagShootInfo {
		_float3 vBulletColor{ 0.f, 0.f, 0.f };
		_float3 vBulletScale{ 0.f, 0.f, 0.f };

		_float	fFireAbleTime{ 0.f };
		_float	fFireSpeed{ 0.f };
		_float	fLifeTime{ 0.f };
		_float	fDamage{ 0.f };
		_float fReloadTime{ 0.f };
		// Bullet Num
		_uint	iBulletNum{ 0 };
	}SHOOTINFO;
protected:
	explicit CShooter() = default;
	explicit CShooter(const CShooter& _rhs) :CCubePawn(_rhs) {}
	virtual ~CShooter() = default;

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
	// MakeBullet
	void MakeBullet(const BULLETTYPE _eBulletType);
	// FieBullet 
	void Fire(const shared_ptr<CCharacter>& _pChar, const shared_ptr<CFloor>& _pFloor, 
		const _float& _fShootAngle);
	// Delet Bullet
	void DeleteFireBullet(const shared_ptr<CBullet>& _pBullet);
protected:
	// UpdateTimes
	void UpdateTimes(const _double& _dTimeDelta);
public:
	void Set_ShootDir(const _float3& _vDir) { this->m_vShootDir = _vDir; }

	SHOOTINFO& Get_ShootInfo() { return m_tShooterInfo; }
	const _bool Is_ReloadTime() const { return m_bReloadTime; }
	const FIREBULLETLIST& Get_BulletList() const { return m_FireBulletList; }

	void Clear() { m_FireBulletList.clear(); }
private:
	// Bullet Saver 
	BULLETSAVER					m_BulletSaver;
	// Fire
	FIREBULLETLIST				m_FireBulletList;
	// TankShooterInfo
	SHOOTINFO						m_tShooterInfo{};
	// Index
	_uint									m_iBulletIndex{ 0 };
	//  BulletType
	BULLETTYPE						m_eCurBulletType{};
	// Bullet Fire Save 
	_double								m_dBulletFireSaveTime{ 0. };
	// Reloadsave
	_double								m_dReloadSaveTime{ 0 };
	// ReloadTime
	_bool									m_bReloadTime{ false };
	_bool									m_bWaitTime{ false };
	_float3								m_vShootDir{ 0.f, 0.f, 0.f };
};

