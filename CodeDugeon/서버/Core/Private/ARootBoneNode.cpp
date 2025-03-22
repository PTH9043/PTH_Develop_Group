#include "CoreDefines.h"
#include "ARootBoneNode.h"

namespace  Core
{
	ARootBoneNode::ARootBoneNode() :
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

	ARootBoneNode::ARootBoneNode(const ARootBoneNode& _rhs) :
		ABoneNode(_rhs),
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

	void ARootBoneNode::UpdateCombinedMatrix(const _float4x4& _mPivotMatrix)
	{
		m_RootBoneNodeFunc(this, _mPivotMatrix);
	}

	void ARootBoneNode::OnRootBoneNode()
	{
		m_RootBoneNodeFunc = UpdateRootBoneUpdateFunc;
	}

	void ARootBoneNode::OffRootBoneNode()
	{
		m_RootBoneNodeFunc = UpdateBasicBoneUpdateFunc;
	}

	void ARootBoneNode::ResetRootBoneInfo()
	{
		m_vPrevRootBonePos = Vector3::Zero;
		m_vCurRootBonePos = Vector3::Zero;
		m_vMoveRootBonePos = Vector3::Zero;

		m_vCurRootBoneAngle = Vector3::Zero;
		m_vPrevRootBoneAngle = Vector3::Zero;
		m_vMoveRootBoneAngle = Vector3::Zero;
	}

	void ARootBoneNode::UpdateMoveBonePos()
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

	void ARootBoneNode::UpdateRootBoneUpdateFunc(ARootBoneNode* _spRootBoneNode, const _float4x4& _mPivotMatrix)
	{
		_spRootBoneNode->ComputeCombinedMatrix(_mPivotMatrix);
		_spRootBoneNode->UpdateMoveBonePos();
	}

	void ARootBoneNode::UpdateBasicBoneUpdateFunc(ARootBoneNode* _spRootBoneNode, const _float4x4& _mPivotMatrix)
	{
		_spRootBoneNode->ComputeCombinedMatrix(_mPivotMatrix);
	}

	void ARootBoneNode::Free()
	{
	}

}