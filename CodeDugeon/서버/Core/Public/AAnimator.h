#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_AANIMATOR_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_AANIMATOR_H

#include "ACoreObject.h"

BEGIN(Core)
class AAnimation;
class ACollider;
class ABoneNode;
class ARootBoneNode;
class ATransform;
class APawn;

using ANIMCONTAINER = VECTOR<SHPTR<AAnimation>>;
using BONECONTAINER = VECTOR<SHPTR<ABoneNode>>;
using SELECTOR = CONUNORMAP<_string, _int>;
using ANIMEVENTCOLLICONTAINER = CONUNORMAP<_int, SHPTR<ACollider>>;
/*
@ Date: 2024-07-03
@ Writer: 박태현
@ Explain
- 서버에서 사용하는 애니메이터
*/
class CORE_DLL AAnimator final : public ACoreObject {
public:
	AAnimator(SHPTR<ACoreInstance> _spCoreInstance, const _string& _strFolderPath, const _string& _strFileName,
	const _float4x4& _PivotMatrix = _float4x4::Identity);
	AAnimator(const AAnimator& _rhs);
	DESTRUCTOR(AAnimator)
public:
	// Tick Event 
	void TickEvent(APawn* _pPawn, const _string& _strInputTrigger, const _double& _TimeDelta);
	// Tick Event 
	void TickEventToRatio(APawn* _pPawn, const _string& _strInputTrigger, const _double& _dRatio, const _double& _TimeDelta);
	// 애니메이션을 timedelta의 흐름에 따라서 제어하는 함수 (TimeDelta는 현재 게임에서 흐르는 시간)
	void TickAnimation(const _double& _dTimeDelta);
	// 애니메이션을 Tick하면서 Event도 Tick하는 함수
	void TickAnimChangeTransform(CSHPTRREF<ATransform> _spTransform, const _double& _dTimeDelta);
	// 현재 애니메이션을 TimAcc 값에 따라서 제어하는 함수 Evnet도 TimeAcc에 따라 제어한다. 
	void TickAnimToTimeAccChangeTransform(CSHPTRREF<ATransform> _spTransform, const _double& _dTimeDelta, const _double& _TimeAc);
	// 애니메이션을 timeAcc에 따라서 제어하는 함수 (TimAcc는 애니메이션 제어도)
	void UpdateCurAnimationToRatio(const _double& _dRatio);
	// Get BoneNode
	SHPTR<ABoneNode> FindBoneNode(const _string& _strBoneNode) const;
	// Update 
	void UpdateAttackData(const _bool _isCanAttackSituation, CSHPTRREF<ACollider> _spCollider);
	// AnimEvent Reset
	void ResetCurAnimEvent();
	// Set Animation
	void SetAnimation(const _int& _iAnimIndex);
	void SetAnimation(const _string& _strAnimName);
	void SetAnimation(const _int& _iAnimIndex, const _double& _dNextTimeAcc);
	void SetAnimation(const _string& _strAnimName, const _double& _dNextTimeAcc);
	// Change Animation
	void ChangeAnimation(const _int& _iAnimIndex);
	void ChangeAnimation(const _string& _strAnimName);
	// Change Animation, Apply Next Time Acc 
	void ChangeAnimation(const _int& _iAnimIndex, const _double& _dNextTimeAcc);
	void ChangeAnimation(const _string& _strAnimName, const _double& _dNextTimeAcc);

	SHPTR<ACollider> BringAttackCollider(_int _iColliderType);
	SHPTR<ACollider> GetAttackCollider() { return m_spAttackCollisionCollider; }
	_bool IsCollisionAttackCollider(SHPTR<ACollider> _spEnemyCollider);
public: /* get set */
	const _int GetCurAnimIndex() const { return m_iCurAnimIndex; }
	SHPTR<AAnimation> GetCurAnimation() const { return m_spCurAnimation; }
	SHPTR<AAnimation> GetNextAnimation() const { return m_spNextAnimation; }

	void SetSupplyLerpValue(const _float _fSupplyLerpValue) { this->m_fSupplyLerpValue = _fSupplyLerpValue; }
protected:
	void ChangeAnimIndex(const _int& _iAnimIndex, ATOMIC<_int>& _iIndex);
	void ChangeAnimIndex(const _string& _strAnimName, ATOMIC<_int>& _iIndex);
	void SettingCurAnimSituation();
	void SettingNextAnimSituation();
private:
	virtual void Free() override;
private:
	ATOMIC<_int>								m_iCurAnimIndex;
	ATOMIC<_int>								m_iNextAnimIndex;
	_float4x4										m_PivotMatirx;
	_float												m_fSupplyLerpValue;
	ATOMIC<_bool>							m_isCanAttackSituation;

	BONECONTAINER							m_BoneContainer;
	SELECTOR										m_BoneSelector;
	ANIMCONTAINER							m_AnimContainer;
	SELECTOR										m_AnimSelector;

	SHPTR<ARootBoneNode>			m_spRootBoneNode;
	SHPTR<AAnimation>					m_spCurAnimation;
	SHPTR<AAnimation>					m_spNextAnimation;
	ANIMEVENTCOLLICONTAINER	m_AnimEventColliderContainer;
	SHPTR<ACollider>						m_spAttackCollisionCollider;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_AANIMATOR_H