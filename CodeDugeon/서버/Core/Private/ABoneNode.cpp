#include "CoreDefines.h"
#include "ABoneNode.h"
#include "AAnimator.h"

namespace Core
{
	ABoneNode::ABoneNode() :
		m_mOffsetMatrix{ _float4x4::Identity }, m_mTransformMatrix{ _float4x4::Identity },
		m_mCombineTransformMatirx{ _float4x4::Identity }, m_mFinalTransformMatrix{ _float4x4::Identity }
		, m_fDepths{ 0.f }, m_strName{ "" }, m_strParentsName{ "" }, m_spParentsNode{ nullptr }
	{
	}

	ABoneNode::ABoneNode(const ABoneNode& _rhs) :
		m_mOffsetMatrix{ _rhs.m_mOffsetMatrix }, m_mTransformMatrix{ _rhs.m_mTransformMatrix },
		m_mCombineTransformMatirx{ _rhs.m_mCombineTransformMatirx }, m_mFinalTransformMatrix{ _rhs.m_mFinalTransformMatrix }
		, m_fDepths{ _rhs.m_fDepths }, m_strName{ _rhs.m_strName }, m_strParentsName{ _rhs.m_strParentsName }, m_spParentsNode{nullptr}
	{
	}

	_bool ABoneNode::NativeConstruct(std::ifstream& _read)
	{
		_read.read((_char*)&m_mOffsetMatrix, sizeof(_float4x4));
		ReadString(_read, m_strName);
		ReadString(_read, m_strParentsName);
		_read.read((_char*)&m_fDepths, sizeof(_float));
		return true;
	}

	void ABoneNode::FindParents(SHPTR<AAnimator> _spAnimator)
	{
		m_spParentsNode = _spAnimator->FindBoneNode(m_strParentsName);
	}

	void ABoneNode::UpdateCombinedMatrix(const _float4x4& _mPivotMatrix)
	{
		ComputeCombinedMatrix(_mPivotMatrix);
	}

	void ABoneNode::RemoveCombineMatrixData()
	{
		m_mCombineTransformMatirx.Set_Pos(Vector3::Zero);
		m_mCombineTransformMatirx = m_mCombineTransformMatirx.MatrixSetRotationFix(Vector3::Zero);
	}

	void ABoneNode::ComputeCombinedMatrix(const _float4x4& _mPivotMatrix)
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

	void ABoneNode::Free()
	{
	}
}