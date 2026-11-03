#pragma once
#include "CComponent.h"

class CTransform : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
protected:
	explicit CTransform() = default;
	explicit CTransform(const CTransform& _rhs) :CComponent(_rhs) {}
	virtual ~CTransform() = default;
public:
	static shared_ptr<CTransform> Create();
	virtual shared_ptr<CComponent> Clone(const VOIDDATAS& _tDatas) override { return nullptr; }
	virtual void Free() override;
protected:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;
public:
	// Tick
	void Tick(const _double& _dTimeDelta) ;
	// Move 
	void Move_Forward(const _double& _dTimeDelta, const _float& _fSpeed);
	void Move_Back(const _double& _dTimeDelta, const _float& _fSpeed);
	void Move_Left(const _double& _dTimeDelta, const _float& _fSpeed);
	void Move_Right(const _double& _dTimeDelta, const _float& _fSpeed);

	void Move_Pos(const _fvector& _v4TargetPos);
	void Translate_Pos(const _fvector& _v4Pos, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);
	void Translate_Dir(const _fvector& _v4Dir, const _double& _dTimeDelta, const _float& _fSpeed);
	void Translate_Trans(const shared_ptr<CTransform>& _pTransform, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);

	void Translate_PosNotY(const _fvector& _v4Pos, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);
	void Translate_DirNotY(const _fvector& _v4Dir, const _double& _dTimeDelta, const _float& _fSpeed);
	void Translate_TransNotY(const shared_ptr<CTransform>& _pTransform, const _double& _dTimeDelta, const _float& _fSpeed,
		const _float& _fLimitDistance = 0.1f);
	// Turn
	void Rotate_Turn(const _fvector& _vAxis, const _double& _dTimeDelta, const _float& _fSpeed);
	void Rotate_Turn(const _float3& _vAxis, const _double& _dTimeDelta, _float _fSpeed);
	void Rotate_Fix(const _fvector& _vAxis, const _float& _fAngle);
	void Rotate_Fix(const _float3& _vAngle);
	void Chase(const _fvector& _vTargetPos, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance = 0.1f);
	void LookAt(const _fvector& _vTargetPos);
	_float Compute_Distance(const _fvector& _vTargetPos);

public:
	const _vector Get_Right() const { return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_RIGHT][0]); }
	const _vector Get_Up() const { return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_UP][0]); }
	const _vector Get_Look()const { return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_LOOK][0]); }
	const  _vector Get_Pos() const { return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_POSITION][0]); }

	const _float3 Get_RightF3() const { return *((_float3*)&m_WorldMatrix.m[STATE_RIGHT][0]); }
	const _float3 Get_UpF3()const { return *((_float3*)&m_WorldMatrix.m[STATE_UP][0]); }
	const _float3 Get_LookF3()const { return *((_float3*)&m_WorldMatrix.m[STATE_LOOK][0]); }
	const _float3 Get_PosF3() const { return *((_float3*)&m_WorldMatrix.m[STATE_POSITION][0]); }

	const _vector Get_OriginRight() const { return XMLoadFloat4((_float4*)&m_ChangeWorldMatrix.m[STATE_RIGHT][0]); }
	const _vector Get_OriginUp() const { return XMLoadFloat4((_float4*)&m_ChangeWorldMatrix.m[STATE_UP][0]); }
	const _vector Get_OriginLook() const { return XMLoadFloat4((_float4*)&m_ChangeWorldMatrix.m[STATE_LOOK][0]); }
	const _vector Get_OriginPos() const { return XMLoadFloat4((_float4*)&m_ChangeWorldMatrix.m[STATE_POSITION][0]); }

	const _float3 Get_OriginRightF3() const { return *((_float3*)&m_ChangeWorldMatrix.m[STATE_RIGHT][0]); }
	const _float3 Get_OriginUpF3()const { return *((_float3*)&m_ChangeWorldMatrix.m[STATE_UP][0]); }
	const _float3 Get_OriginLookF3()const { return *((_float3*)&m_ChangeWorldMatrix.m[STATE_LOOK][0]); }
	const _float3 Get_OriginPosF3()const { return *((_float3*)&m_ChangeWorldMatrix.m[STATE_POSITION][0]); }

	const _float3 Get_Scale() const;
	const _float3 Get_ParentsScale() const;
	const _float4x4& Get_WorldFloat4x4() const { return m_WorldMatrix; }
	const _matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	const _float4x4 Get_WorldFloat4x4_TP() const;
	const _float4x4 Get_WorldFloat4x4Inv() const;
	const _matrix Get_WorldMatrixInv() const { return  XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)); }

	const _float4x4 Get_ParentsToWorldFloat4x4() const;
	const _matrix Get_ParentsToWorldMatrix() const;
	const _float4x4 Get_ParentsToWorldFloat4x4_TP() const;
	const _float4x4 Get_ParentsToWorldFloat4x4Inv() const ;
	const _matrix Get_ParentsToWorldMatrixInv() const;

	void Set_Right(const _fvector& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_RIGHT][0], _vState); }
	void Set_Up(const _fvector& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_UP][0], _vState); }
	void Set_Look(const _fvector& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_LOOK][0], _vState); }
	void Set_Pos(const _fvector& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_POSITION][0], _vState); }

	void Set_RightF3(const _float3& _vState) { *((_float3*)&m_WorldMatrix.m[STATE_RIGHT][0]) = _vState; }
	void Set_UpF3(const _float3& _vState) { *((_float3*)&m_WorldMatrix.m[STATE_UP][0]) = _vState; }
	void Set_LookF3(const _float3& _vState) { *((_float3*)&m_WorldMatrix.m[STATE_LOOK][0]) = _vState; }
	void Set_PosF3(const _float3& _vState) { *((_float3*)&m_WorldMatrix.m[STATE_POSITION][0]) = _vState; }

	void Set_Right(const _matrix& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_RIGHT][0], XMVector3Normalize(_vState.r[0]) * Get_Scale().x); }
	void Set_Up(const _matrix& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_UP][0], XMVector3Normalize(_vState.r[1]) * Get_Scale().y); }
	void Set_Look(const _matrix& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_LOOK][0], XMVector3Normalize(_vState.r[2]) * Get_Scale().z); }
	void Set_Pos(const _matrix& _vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[STATE_POSITION][0], _vState.r[3]); }
	void Set_Scale(const _float3& _vScale);
	// Bind World Matrix
	void Bind_WorldMatrix(const _matrix& _fMatrix);

	const _float NearToDistance(const _float3& _vPos);
	const _float NearToDistanceSq(const _float3& _vPos);
private:
	// Update Matrix
	void UpdateMatrix();
public:
	// Set Parents Tr
	void Set_ParentsTransform(const weak_ptr<CTransform>& _pWeakPtr);

	void Set_NotApplyRotate(const _bool _bActive) { this->m_bNotApplyRotate = _bActive; }
	void Set_NotApplyPos(const _bool _bActive) { this->m_bNotApplyPos = _bActive; }
	void Set_NotApplyScale(const _bool _bActive) { this->m_bNotApplyScale = _bActive; }
private:
	// World Matrix
	_float4x4								m_WorldMatrix = IDENTIFY;
	_float4x4								m_ChangeWorldMatrix = IDENTIFY;
	_float3									m_vScale{ 0.f, 0.f,0.f };
	// Parents Transform
	weak_ptr<CTransform>		m_pParentsTransform{};

	_bool										m_bNotApplyRotate{ false };
	_bool										m_bNotApplyPos{ false };
	_bool										m_bNotApplyScale{ false };
};

