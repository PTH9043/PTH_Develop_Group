#include "EngineDefine.h"
#include "URootBoneNode.h"

URootBoneNode::URootBoneNode() : 
	m_RootBoneNodeFunc{ UpdateBasicBoneUpdateFunc },
	m_vCurRootBonePos{},
	m_vPrevRootBonePos{},
	m_vMoveRootBonePos{},
	m_vCurRootBoneAngle{},
	m_vPrevRootBoneAngle{},
	m_vMoveRootBoneAngle{},
	m_OutMatrix{}
{
	SetRootBoneNode(true);
}

URootBoneNode::URootBoneNode(const URootBoneNode& _rhs) : 
	UBoneNode(_rhs), 
m_RootBoneNodeFunc{ UpdateBasicBoneUpdateFunc },
m_vCurRootBonePos{},
m_vPrevRootBonePos{},
m_vMoveRootBonePos{},
m_vCurRootBoneAngle{},
m_vPrevRootBoneAngle{},
m_vMoveRootBoneAngle{},
m_OutMatrix{}
{
}

SHPTR<UBoneNode> URootBoneNode::Clone()
{
	SHPTR<URootBoneNode> spRootBoneNode = { CloneThis<URootBoneNode>(*this) };
	return spRootBoneNode;
}

void URootBoneNode::Free()
{
}

HRESULT URootBoneNode::NativeConstruct(const _wstring& _wstrBoneName)
{
	SetDepths(-1.f);
	SetName(_wstrBoneName);
	return S_OK;
}

void URootBoneNode::UpdateCombinedMatrix(const _float4x4& _mPivotMatrix)
{
	m_RootBoneNodeFunc(this, _mPivotMatrix);
}

void URootBoneNode::OnRootBoneNode()
{
	m_RootBoneNodeFunc = UpdateRootBoneUpdateFunc;
}

void URootBoneNode::OffRootBoneNode()
{
	m_RootBoneNodeFunc = UpdateBasicBoneUpdateFunc;
}

void URootBoneNode::ResetRootBoneInfo()
{
	m_vPrevRootBonePos = _float3::Zero;
	m_vCurRootBonePos = _float3::Zero;
	m_vMoveRootBonePos = _float3::Zero;

	m_vCurRootBoneAngle = _float3::Zero;
	m_vPrevRootBoneAngle = _float3::Zero;
	m_vMoveRootBoneAngle = _float3::Zero;
}

void URootBoneNode::UpdateMoveBonePos()
{
	DirectX::PTH::OUTMATRIX OutMatrix;
	m_OutMatrix = _float4x4::Get_OutMatrix(GetCombineMatrix());

	m_vPrevRootBoneAngle = m_vCurRootBoneAngle;
	m_vCurRootBoneAngle = m_OutMatrix.vRot;
	m_vMoveRootBoneAngle = m_vCurRootBoneAngle;

	m_vPrevRootBonePos = m_vCurRootBonePos;
	m_vCurRootBonePos = GetCombineMatrixPos();
	m_vMoveRootBonePos = m_vCurRootBonePos - m_vPrevRootBonePos;
	RemoveCombineMatrixData();
}

void URootBoneNode::UpdateRootBoneUpdateFunc(URootBoneNode* _spRootBoneNode, const _float4x4& _mPivotMatrix)
{
	_spRootBoneNode->ComputeCombinedMatrix(_mPivotMatrix);
	_spRootBoneNode->UpdateMoveBonePos();
}

void URootBoneNode::UpdateBasicBoneUpdateFunc(URootBoneNode* _spRootBoneNode, const _float4x4& _mPivotMatrix)
{
	_spRootBoneNode->ComputeCombinedMatrix(_mPivotMatrix);
}
