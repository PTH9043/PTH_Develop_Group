#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_AROOTBONENODE_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_AROOTBONENODE_H

#include "ABoneNode.h"

BEGIN(Core)
class ATransform;

class ARootBoneNode final : public ABoneNode {
	using ROOTBONEUPDATEFUNC = std::function<void(ARootBoneNode*, const _float4x4&)>;
public:
	ARootBoneNode();
	ARootBoneNode(const ARootBoneNode& _rhs);
	DESTRUCTOR(ARootBoneNode)
public:
	virtual void UpdateCombinedMatrix(const _float4x4& _mPivotMatrix) override;
	void OnRootBoneNode();
	void OffRootBoneNode();
	void ResetRootBoneInfo();
public: /* Get Set */
	const Vector3& GetCurRootBonePos() const { return m_vCurRootBonePos; }
	const Vector3& GetPrevRootBonePos() const { return m_vPrevRootBonePos; }
	const Vector3& GetMoveRootBonePos() const { return m_vMoveRootBonePos; }

	const Vector3& GetCurRootBoneAngle() const { return m_vCurRootBoneAngle; }
	const Vector3& GetPrevRootBoneAngle() const { return m_vPrevRootBoneAngle; }
	const Vector3& GetMoveRootBoneAngle() const { return m_vMoveRootBoneAngle; }
private:
	void UpdateMoveBonePos();
	static void UpdateRootBoneUpdateFunc(ARootBoneNode* _spRootBoneNode, const _float4x4& _mPivotMatrix);
	static void UpdateBasicBoneUpdateFunc(ARootBoneNode* _spRootBoneNode, const _float4x4& _mPivotMatrix);
private:
	virtual void Free() override;
private:
	Vector3										m_vCurRootBonePos;
	Vector3										m_vPrevRootBonePos;
	Vector3										m_vMoveRootBonePos;

	Vector3										m_vCurRootBoneAngle;
	Vector3										m_vPrevRootBoneAngle;
	Vector3										m_vMoveRootBoneAngle;

	DirectX::PTH::OUTMATRIX		m_OutMatrix;
	ROOTBONEUPDATEFUNC		m_RootBoneNodeFunc;

};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_AROOTBONENODE_H