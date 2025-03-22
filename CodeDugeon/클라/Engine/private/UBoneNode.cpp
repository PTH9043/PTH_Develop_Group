#include "EngineDefine.h"
#include "UBoneNode.h"
#include "UModel.h"
#include "UMethod.h"

_wstring					UBoneNode::BASE_BONENODE_NAME{ L"" };


UBoneNode::UBoneNode() :
	m_mOffsetMatrix{ _float4x4::Identity },
	m_mTransformMatrix{ _float4x4::Identity },
	m_mCombineTransformMatirx{ _float4x4::Identity },
	m_spParentsNode{ nullptr },
	m_wstrName{ L"" },
	m_wstrParetnsName{ L"" },
	m_fDepths{ 0.f },
	m_mFinalTransformMatrix{_float4x4::Identity},
	m_isRootBoneNode{false}
{
}

UBoneNode::UBoneNode(const UBoneNode& _rhs) :
	m_mOffsetMatrix{ _rhs.m_mOffsetMatrix },
	m_mTransformMatrix{ _rhs.m_mTransformMatrix },
	m_mCombineTransformMatirx{ _rhs.m_mCombineTransformMatirx },
	m_spParentsNode{ nullptr },
	m_wstrName{ _rhs.m_wstrName },
	m_wstrParetnsName{ _rhs.m_wstrParetnsName },
	m_fDepths{ _rhs.m_fDepths },
	m_mFinalTransformMatrix{ _float4x4::Identity },
	m_isRootBoneNode{ _rhs.m_isRootBoneNode }
{
}

SHPTR<UBoneNode> UBoneNode::Clone()
{
	return { CloneThis<UBoneNode>(*this) };
}

void UBoneNode::Free()
{
}

HRESULT UBoneNode::NativeConstruct(const BONENODEDESC& _stBoneNodeDesc)
{
	m_wstrName = _stBoneNodeDesc.wstrName;
	m_wstrParetnsName = _stBoneNodeDesc.wstrParentsName;
	m_fDepths = _stBoneNodeDesc.fDepth;
	m_mOffsetMatrix = _stBoneNodeDesc.mOffsetMatrix;
	m_mTransformMatrix = _stBoneNodeDesc.mTransformMatrix;

	return S_OK;
}

void UBoneNode::FindParents(CSHPTRREF<UModel> _spModel)
{
	m_spParentsNode = _spModel->FindBoneNode(m_wstrParetnsName);
}

void UBoneNode::UpdateCombinedMatrix(const _float4x4& _mPivotMatrix)
{
	ComputeCombinedMatrix(_mPivotMatrix);
}

void UBoneNode::SaveServerData(std::ofstream& _Saves)
{
	_Saves.write((_char*)&m_mOffsetMatrix, sizeof(_float4x4));
	UMethod::SaveString(_Saves, UMethod::ConvertWToS(m_wstrName));
	UMethod::SaveString(_Saves, UMethod::ConvertWToS(m_wstrParetnsName));
	_Saves.write((_char*)&m_fDepths, sizeof(_float));
}

void UBoneNode::RemoveCombineMatrixData()
{
	m_mCombineTransformMatirx.Set_Pos(_float3::Zero);
	m_mCombineTransformMatirx = m_mCombineTransformMatirx.MatrixSetRotationFix(_float3::Zero);
}

void UBoneNode::ComputeCombinedMatrix(const _float4x4& _mPivotMatrix)
{
	if (nullptr != m_spParentsNode)
	{
		m_mCombineTransformMatirx = m_mTransformMatrix * m_spParentsNode->GetCombineMatrix();
	}
	else
	{
		m_mCombineTransformMatirx = m_mTransformMatrix;
	}

	m_mFinalTransformMatrix = m_mOffsetMatrix * m_mCombineTransformMatirx * _mPivotMatrix;
	m_mFinalTransformMatrix = m_mFinalTransformMatrix.Transpose();
}
