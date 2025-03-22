#pragma once
#include "Component.h"

/* XMVECTOR, MATRIX는 &연산을 절대 쓰지 않는다.  */

BEGIN(Engine)
class CNavigation;
class CCell;

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
private:
	explicit CTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CTransform(const CTransform& _rhs);
	virtual ~CTransform() = default;
public:
	CCell* Get_CurCell() { return m_pCell; }

	_vector Get_Right() { return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_RIGHT][0]); }
	_vector Get_Up() { return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_UP][0]); }
	_vector Get_Look() { return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_LOOK][0]); }
	_vector Get_Pos() { return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_POSITION][0]); }

	_float3 Get_RightF3() { return *((_float3*)&m_WorldMatrix.m[STATE_RIGHT][0]); }
	_float3 Get_UpF3() { return *((_float3*)&m_WorldMatrix.m[STATE_UP][0]); }
	_float3 Get_LookF3() { return *((_float3*)&m_WorldMatrix.m[STATE_LOOK][0]); }
	_float3 Get_PosF3() { return *((_float3*)&m_WorldMatrix.m[STATE_POSITION][0]); }

	_float3 Get_Scale();
	const _float4x4& Get_WorldFloat4x4() const { return m_WorldMatrix; }
	_matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	_float4x4 Get_WorldFloat4x4_TP() const;
	_float4x4 Get_WorldFloat4x4Inv();
	_matrix Get_WorldMatrixInv() { return  XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)); }

	const _bool& Is_CollisionNav() const { return m_bCollisionNav; }

	void Set_Right(const _fvector& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_RIGHT][0], _vState); }
	void Set_Up(const _fvector& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_UP][0], _vState); }
	void Set_Look(const _fvector& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_LOOK][0], _vState); }
	void Set_Pos(const _fvector& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_POSITION][0], _vState); }

	void Set_RightF3(const _float3& _vState) { *((_float3*)&m_WorldMatrix.m[STATE_RIGHT][0]) = _vState; }
	void Set_UpF3(const _float3& _vState) { *((_float3*)&m_WorldMatrix.m[STATE_UP][0]) = _vState; }
	void Set_LookF3(const _float3& _vState) { *((_float3*)&m_WorldMatrix.m[STATE_LOOK][0]) = _vState; }
	void Set_PosF3(const _float3& _vState) { *((_float3*)&m_WorldMatrix.m[STATE_POSITION][0]) = _vState; }

	void Set_Right(const _fmatrix& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_RIGHT][0], XMVector3Normalize(_vState.r[0]) * Get_Scale().x); }
	void Set_Up(const _fmatrix& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_UP][0], XMVector3Normalize(_vState.r[1]) * Get_Scale().y); }
	void Set_Look(const _fmatrix& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_LOOK][0], XMVector3Normalize(_vState.r[2]) * Get_Scale().z); }
	void Set_Pos(const _fmatrix& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_POSITION][0], _vState.r[3]); }

	void Set_Scale(const _float3& _vScale);
	// Bind World Matrix
	void Bind_WorldMatrix(const _fmatrix& _fMatrix);
public:
	// NativeConstruct Prototype
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;

	void Update_Nav(CNavigation* _pNavigation) { this->m_pNavigation = _pNavigation; }
public:
	// Move 
	void Move_Forward(const _double& _dTimeDelta, const _float& _fSpeed);
	void Move_Back(const _double& _dTimeDelta, const _float& _fSpeed);
	void Move_Left(const _double& _dTimeDelta, const _float& _fSpeed);
	void Move_Right(const _double& _dTimeDelta, const _float& _fSpeed);
	
	void Move_Pos(const _fvector& _v4TargetPos);
	void Move_PosNotNavigation(const _fvector& _v4TargetPos);
	_bool Move_Pos_NotNavigateY(const _fvector& _v4TargetPos);

	void Translate_Pos(const _fvector& _v4Pos, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);
	void Translate_Dir(const _fvector& _v4Dir, const _double& _dTimeDelta, const _float& _fSpeed);
	void Translate_DirNotNavigation(const _fvector& _v4Dir, const _double& _dTimeDelta, const _float& _fSpeed);
	void Translate_Trans(CTransform* _pTransform, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);
	void Translate_DirSliding(const _fvector& _v4Dir, const _double& _dTimeDelta, const _float& _fSpeed);

	// Turn
	void Rotate_Turn(const _fvector& _vAxis, const _double& _dTimeDelta, const _float& _fSpeed);
	void Rotate_Fix(const _fvector& _vAxis, const _float& _fAngle);
	void Rotate_Turn(const _float3& _vAngle);
	void Rotate_TurnToNotFix(const _float3& _vAngle);
	void Chase(const _fvector& _vTargetPos, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance = 0.1f);
	void LookAt(const _fvector& _vTargetPos);
	void LookBack(const _fvector& _vTargetPos);
	_float Compute_Distance(const _fvector& _vTargetPos);

	_bool Update_Navigation_Cell(const _fvector& vTargetPos);

	CNavigation* Get_Navigation() { return m_pNavigation; }
	
protected:
	_bool Is_Running_Nav(_vector& _vPos, _float* _fY = nullptr);
private:
	// WorldMatrix
	_float4x4				m_WorldMatrix = _float4x4();

	CNavigation*		m_pNavigation = nullptr;
	CCell*						m_pCell = nullptr;
	_bool						m_bCollisionNav = false;
public:
	// Create
	static CTransform* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	// Clone
	virtual CComponent* Clone(void* _pArg) override;
	// override
	virtual void Free() override;
};

END