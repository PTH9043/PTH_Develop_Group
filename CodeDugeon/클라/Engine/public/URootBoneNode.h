#pragma once 
#include "UBoneNode.h"

BEGIN(Engine)
class UModel;
class UTransform;
/*
@ Date: 2024-02-12, Writer: 박태현
@ Explain
- 애니메이션의 움직임을 실제 Transform으로 가져오기 위한 RootBone클래스이다. 
*/
class URootBoneNode final : public UBoneNode {
	using ROOTBONEUPDATEFUNC = std::function<void(URootBoneNode*, const _float4x4&)>;
public:
	URootBoneNode();
	URootBoneNode(const URootBoneNode& _rhs);
	DESTRUCTOR(URootBoneNode)
public: /* Get Set */
	const _float3& GetCurRootBonePos() const { return m_vCurRootBonePos; }
	const _float3& GetPrevRootBonePos() const{ return m_vPrevRootBonePos; }
	const _float3& GetMoveRootBonePos() const{ return m_vMoveRootBonePos; }

	const _float3& GetCurRootBoneAngle() const { return m_vCurRootBoneAngle; }
	const _float3& GetPrevRootBoneAngle() const { return m_vPrevRootBoneAngle; }
	const _float3& GetMoveRootBoneAngle() const { return m_vMoveRootBoneAngle; }
public:
	virtual SHPTR<UBoneNode> Clone() override;
	virtual void Free() override;
	using UBoneNode::NativeConstruct;
	HRESULT NativeConstruct(const _wstring& _wstrBoneName);

	virtual void UpdateCombinedMatrix(const _float4x4& _mPivotMatrix) override;
	void OnRootBoneNode();
	void OffRootBoneNode();
	void ResetRootBoneInfo();
private:
	void UpdateMoveBonePos();
	static void UpdateRootBoneUpdateFunc(URootBoneNode* _spRootBoneNode, const _float4x4& _mPivotMatrix);
	static void UpdateBasicBoneUpdateFunc(URootBoneNode* _spRootBoneNode, const _float4x4& _mPivotMatrix);
private:
	_float3										m_vCurRootBonePos;
	_float3										m_vPrevRootBonePos;
	_float3										m_vMoveRootBonePos;

	_float3										m_vCurRootBoneAngle;
	_float3										m_vPrevRootBoneAngle;
	_float3										m_vMoveRootBoneAngle;

	DirectX::PTH::OUTMATRIX		m_OutMatrix;
	ROOTBONEUPDATEFUNC		m_RootBoneNodeFunc;
};

END