#pragma once
#include "Client_Defines.h"
#include "CCamera.h"

BEGIN(Engine)
class CGameObject;
class CTransform;
class CNavigation;
class CCell;
class CCollider;
END

BEGIN(Client)
class CCinematicCamera;

class CCamera_Free final : public CCamera
{
private:
	explicit CCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCamera_Free(const CCamera_Free& _rhs);
	virtual ~CCamera_Free() = default;
public:
	const _float& Get_PlayerToAngle() const { return m_fPlayerToAngle; }
	const _float3& Get_SupplementPos() const { return m_v3SupplementPos; }
public: 
	void Set_TargetTransform(CTransform* _pTransform) { this->m_pTargetTransform = _pTransform; }
	void Set_OffsetPosition(const _vector& _vPos) { XMStoreFloat3(&m_v3OffsetPos, _vPos + XMVectorSet(0.f, 2.f, 0.f, 0.f)); }
	void Set_Speed(const _float& _fSpeed) { this->m_tCameraDesc.fSpeed = _fSpeed; }
	void Set_Move(const _bool& _bIsMove) { this->m_bIsMove = _bIsMove; }
	void Target_Mode(const _bool& _bIsTargetMode, _fvector _vPos, _fvector _vTargetPos );

	void Insert_CinematicCam(CCinematicCamera* _pCamera);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:
	// Calculate Pos
	void Calculate_Pos();
public:
	_float PlayerToDot_DirToSelfLook();
	_float PlayerToDot_DirToSelfRight();
	_float PlayerToDot_DirToSelfLookConverter();
protected:
	// Rotation
	void Rotation(const _double& _dDeltaSeconds);
	// Move
	void Move(const _double& _dDeltaSeconds);
	// Collision Wall
	void Collision_Wall();
	// Restrict 
	_float CampAngle(_float _fAngle, const _float& _fMin, const _float& _fMax, const _float& fRadius);
protected:
	class CSkyBox*							m_pSkyBox = nullptr;
	class CStage*								m_pStage = nullptr;
	class CRegion*							m_pRegion = nullptr;
	CNavigation*								m_pNavigation = nullptr;
	CCell*												m_pCell = nullptr;
	CCollider*										m_pCollider = nullptr;
	// CCinematic Camera
	list<CCinematicCamera*>		m_pCinematicCamList;
private:
	CTransform*								m_pTargetTransform = nullptr;
	// OffSet Matrix	
	_float3											m_v3OffsetPos = _float3(0.f, 0.f, 0.f);
	// Cam To Angle
	_float												m_fPlayerToAngle = 0.f;
	// Rotation Angles
	_float												m_fRotationXAngle = 0.f;
	_float												m_fRotationYAngle = 0.f;
	// Goal Pos
	_float3											m_v3GoalPos = _float3(0.f, 0.f, 0.f);
	// Supplement Pos
	_float3											m_v3OldSupplementPos = _float3(0.f, 0.f, 0.f);
	_float3											m_v3SupplementPos = _float3(0.f, 0.f, 0.f);
	// Mouse Value
	_float												m_fMouseX = 0.f;
	_float												m_fMouseY = 0.f;
	// Change Value
	_bool												m_bChangeValue = false;
	_bool												m_bIsMove = true;

	_bool												m_bYMove = false;
	_bool												m_bXMove = false;
	_bool												m_bTargetMode = false;
	_float3											m_v3Fix_LockOnPos = _float3(0.f, 0.f, 0.f);
	_float3											m_v3Fix_TargetPos = _float3(0.f, 0.f, 0.f);
public:
	static CCamera_Free* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
