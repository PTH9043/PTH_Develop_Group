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
class CGameScene;

class CTank : public CCharacter {
public:
	typedef struct tagTankDesc : public CHARDESC {
		_float fPosinRotateSpeed = 0.f;
		_float fTankMoveSpeed = 0.f;
		_float fTankRotateSpeed = 0.f;
	}TANKDESC;
protected:
	explicit CTank() : CCharacter(CHAR_PLAYER){}
	CTank(const CTank& _rhs) :CCharacter(_rhs) {}
	virtual ~CTank() = default;

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
	void ControlTank(const _double& _dTimeDelta);
	// ControlCamera
	void ControlCamera(const _double& _dTimeDelta);
	// Damaged
	virtual void Damaged(const shared_ptr<CCharacter>& _pChar) override;
public:
	void Set_GameScene(const shared_ptr<CGameScene>& _pGameScene) { this->m_pGameScene = _pGameScene; }
	// Set Camera
	void Set_MainCamera(const shared_ptr<CMainCamera>& _pCamera);
public:
	void Reset() { m_dShockTime = 0.; m_iShockCountDown = SHOCK_CNT; m_bShockState = false; }
	void Set_ShockState(const _bool _bShockState) { this->m_bShockState = _bShockState; }

	const shared_ptr<CCubeBuffer>& Get_CubeBuffer() const { return m_pCubeBuffer; }
	const _uint& Get_ShockCountDown() const { return m_iShockCountDown; }
	const _bool& Is_ShockingState() const { return m_bShockState; }
	const shared_ptr<CShooter>& Get_Shooter() { return m_pTankShooter; }
private:
	// Game Scene
	weak_ptr<CGameScene>			m_pGameScene{};
	// CubeBuffer
	shared_ptr<CCubeBuffer>			m_pCubeBuffer = nullptr;
	// FowardShow
	shared_ptr<CCube>					m_pFowardShow = nullptr;
	// Head
	shared_ptr<CControlHead>		m_pTankHead = nullptr;
	// Tank Posin
	shared_ptr< CPosin>					m_pTankPosin = nullptr;
	// TankShooter
	shared_ptr<CShooter>				m_pTankShooter = nullptr;
	// Main Cam
	shared_ptr<CMainCamera>		m_pMainCamera = nullptr;
	// Empty Object
	shared_ptr<CEmptyObject>		m_pCamerFollowObject = nullptr;
	// Tank Value
	_float												m_fTankMoveSpeed = 0.f;
	_float												m_fTankRotateSpeed = 0.f;
	_bool												m_bDontMove = false;
	_bool												m_bShockState = false;
	const _double								SHOCK_TIME = 3.;
	_double											m_dShockTime{ 0 };
	// Prev Pos
	_float3											m_vPrevPos{ 0.f, 0.f, 0.f };
	// Shock Count Down
	static const _uint							SHOCK_CNT = 7;
	_uint												m_iShockCountDown{ SHOCK_CNT };
};

