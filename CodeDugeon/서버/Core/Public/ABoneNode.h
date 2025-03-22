#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ABONENODE_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ABONENODE_H

#include "ACoreBase.h"

BEGIN(Core)
class AAnimator;

/*
@ Date: 2024-07-03
@ Writer: 박태현
@ Explain
- 서버에서 사용하는 본 노드
*/
class ABoneNode : public ACoreBase {
public:
	ABoneNode();
	ABoneNode(const ABoneNode& _rhs);
	DESTRUCTOR(ABoneNode)
public:
	_bool NativeConstruct(std::ifstream& _read);
	void FindParents(SHPTR<AAnimator> _spAnimator);
	virtual void UpdateCombinedMatrix(const _float4x4& _mPivotMatrix);
public: /* get set*/
	const _float4x4& GetOffsetMatrix() const { return m_mOffsetMatrix; }
	const _float4x4& GetTransformMatrix() const { return m_mTransformMatrix; }
	const _float4x4& GetCombineMatrix() const { return m_mCombineTransformMatirx; }
	const _float4x4& GetFinalTranformMatrix() const { return m_mFinalTransformMatrix; }
	const _float GetDepth() const { return m_fDepths; }
	const _string& GetName() const { return m_strName; }

	void SetTransformMatrix(const _float4x4& _mTranformMatrix)
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);
		this->m_mTransformMatrix = _mTranformMatrix; 
	}
	void SetOffsetmatrix(const _float4x4& _mOffsetMatrix) 
	{
		std::atomic_thread_fence(std::memory_order_seq_cst);
		this->m_mOffsetMatrix = _mOffsetMatrix; 
	}
protected:
	void RemoveCombineMatrixData();
	void ComputeCombinedMatrix(const _float4x4& _mPivotMatrix);

	const Vector3& GetCombineMatrixLook() const { return *((Vector3*)&m_mCombineTransformMatirx.m[DirectX::PTH::MATROW_LOOK][0]); }
	const Vector3& GetCombineMatrixPos() const { return *((Vector3*)&m_mCombineTransformMatirx.m[DirectX::PTH::MATROW_POS][0]); }
private:
	virtual void Free() override;
private:
	_float4x4							m_mOffsetMatrix;
	_float4x4							m_mTransformMatrix;
	_float4x4							m_mCombineTransformMatirx;
	_float4x4							m_mFinalTransformMatrix;
	_float									m_fDepths;

	_string								m_strName;
	_string								m_strParentsName;

	SHPTR<ABoneNode>		m_spParentsNode;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_ABONENODE_H