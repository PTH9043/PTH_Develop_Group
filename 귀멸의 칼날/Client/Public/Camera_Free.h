#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CCamera_Free : public CCamera
{
private:
	explicit CCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCamera_Free(const CCamera_Free& _rhs);
	virtual ~CCamera_Free() = default;

public:
	CCamera::CAMDESC Get_CamDesc();
	const _bool& Get_IsFree() const { return m_bIsFree; }
	
	// 일정 조건 만족시 카메라를 움직인다.
	void	Set_CamMove(const _bool& bIsMove) { m_bIsAttack = bIsMove; }
	void	Set_HitCount(const _uint& iHitCount) { m_iHitCount = iHitCount; }

	// 흔들리는 시간과 강도에 따라 카메라가 흔들린다.
	void	Start_CameraShake(_float fShakeTime, _float fMagnitude);
	void	Set_StoryEnding(_bool bIsEnding) { m_bStoryEnding = bIsEnding; }

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	virtual void Extern_ImGuiClass() override;

private:
	// Control
	void	StoryCam_Control(const _double& _dTimeDelta);

	// Move
	void	FreeCam_Move(const _double& _dTimeDelta);
	void	BattleCam_Move(const _double& _dTimeDelta);
	void	TeOniCam_Move(const _double& _dTimeDelta);
	void	StoryCam_Move(const _double& _dTimeDelta);
	void	RattanCam_Move(const _double& _dTimeDelta);
	void	StoryBattleCam_Move(const _double& _dTimeDelta);
	void	ShakeCam_Move(const _double& _dTimeDelta);

private:
	// Check Collision MapObj
	void	Check_Collision_MapObj();
	// SetUp Components
	virtual HRESULT SetUp_Components();

private:
	_bool		m_bIsFree = false;
	CCollider*	m_pColliderCom = nullptr;

private:
	// 배틀 카메라
	_bool		m_bIsAttack = false;
	// 배틀 카메라를 움직이는 조건을 처리하기위한 히트 카운트
	_uint		m_iHitCount = 0;
	_float3		m_vPreCenter = _float3(0.f, 0.f, 0.f);

private:
	// 흔들리는 카메라
	_bool		m_bIsShake = false;
	_float		m_fShakeMagnitude = 1.f;
	_float3		m_vOriginalPos = _float3(0.f, 0.f, 0.f);
	CUSTIMER	m_ShakeTimer;

	CRenderer*	m_pRendererCom = nullptr;

private:
	_bool		m_bStoryEnding = false;
	CUSTIMER	m_ZoomTimer;

public:
	static CCamera_Free* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END