#include "ToolDefines.h"
#include "TAssimpBoneNode.h"
#include "UMethod.h"

TAssimpBoneNode::TAssimpBoneNode() :
	m_spParentsBoneNode{ nullptr },
	m_wstrBoneName{ L"" },
	m_mTransformMatrix{ _float4x4::Identity },
	m_mSaveTransformMatrix{ _float4x4::Identity },
	m_mCombinedTransformMatrix{ _float4x4::Identity },
	m_mOffsetMatrix{ _float4x4::Identity },
	m_fDepth{ 0.f }
{
}

TAssimpBoneNode::TAssimpBoneNode(const TAssimpBoneNode& _rhs) :
	m_spParentsBoneNode{ _rhs.m_spParentsBoneNode },
	m_wstrBoneName{ _rhs.m_wstrBoneName },
	m_mTransformMatrix{ _rhs.m_mTransformMatrix },
	m_mSaveTransformMatrix{ _rhs.m_mSaveTransformMatrix },
	m_mCombinedTransformMatrix{ _rhs.m_mCombinedTransformMatrix },
	m_mOffsetMatrix{ _rhs.m_mOffsetMatrix },
	m_fDepth{ _rhs.m_fDepth }
{
}

void TAssimpBoneNode::Free()
{
}

HRESULT TAssimpBoneNode::NativeConstruct(CSHPTRREF<TAssimpBoneNode> _spParentsBone, const _string& _strName, const _float4x4& _mTransformMatrix, const _float& _fDepths)
{
	m_wstrBoneName = UMethod::ConvertSToW(_strName);
	m_mTransformMatrix = _mTransformMatrix;
	m_mSaveTransformMatrix = _mTransformMatrix;
	m_fDepth = _fDepths;
	m_spParentsBoneNode = _spParentsBone;
	return S_OK;
}

void TAssimpBoneNode::UpdateCombineMatrix()
{
	if (nullptr != m_spParentsBoneNode) {
		m_mCombinedTransformMatrix = m_mTransformMatrix *
			m_spParentsBoneNode->m_mCombinedTransformMatrix;
	}
	else {
		m_mCombinedTransformMatrix = m_mTransformMatrix;
	}
}

void TAssimpBoneNode::GetData(BONENODEDESC& _stBoneNodeDesc)
{
	_stBoneNodeDesc.mTransformMatrix = m_mSaveTransformMatrix;
	_stBoneNodeDesc.fDepth = m_fDepth;
	_stBoneNodeDesc.mOffsetMatrix = m_mOffsetMatrix;
	_stBoneNodeDesc.wstrName = m_wstrBoneName;
	if (nullptr != m_spParentsBoneNode)
		_stBoneNodeDesc.wstrParentsName = m_spParentsBoneNode->m_wstrBoneName;
}
