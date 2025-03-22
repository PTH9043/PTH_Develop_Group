#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_AANIMATION_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_AANIMATION_H

#include "ACoreBase.h"

BEGIN(Core)
class AAnimator;
class AChannel;
class AAnimEvent;
class APawn;

using CHANNELCONTAINER = VECTOR<SHPTR<AChannel>>;
using ANIMFASTSECTIONS = VECTOR<ANIMFASTSECTION>;
using ANIMEVENTCONTAINER = CONUNORMAP<ANIMEVENTTYPE, VECTOR<SHPTR<AAnimEvent>>>;
/*
@ Date: 2024-07-03
@ Writer: 박태현
@ Explain
- 서버에서 사용하는 애니메이션 클래스
*/
class CORE_DLL AAnimation final : public ACoreBase {
public:
	AAnimation();
	DESTRUCTOR(AAnimation)
public:
	_bool NativeConstruct(AAnimator* _pAnimator, std::ifstream& _read);
	// 애니메이션과 연결된 뼈 정보들을 업데이트
	void UpdateBoneMatrices(const _double& _dTimeDelta);
	// TimeAcc로 애니메이션과 연결된 뼈 정보들을 업데이트
	void UpdateboneMatricesToRatio(const _double& _Ratio);
	// 다음 애니메이션으로 변경
	void UpdateNextAnimTransformMatrices(const _double& _dTimeDelta, const _float _fSupplyValue, CSHPTRREF<AAnimation> _spAnimation);
	// 해당 프레임으로 애니메이션 변경
	void UpdateTimeAccToChannelIndex(const _double& _dTimeAcc);
	// Animation Event Tick 
	void TickAnimEvent(APawn* _pPawn, AAnimator* _pAnimator, const _double& _TimeDelta, const _string& _strInputTrigger);
	void TickAnimEvent(APawn* _pPawn, AAnimator* _pAnimModel, const _double& _TimeDelta, 
		const _double& _Ratio, const _string& _strInputTrigger);
	void ResetData();
	void ResetAnimChangeEventNode();

	void LoadAnimSectionData(const _string& _strFolderPath);
	void LoadAnimEventData(AAnimator* _pAnimator, const _string& _strFolderPath);
public:
	const _string GetAnimName() const { return m_strName; }
	const _bool IsFinishAnim() const { return m_isFinishAnimation; }
	const _bool IsSupplySituation() const { return m_isSupplySituation; }
	const _double GetDuration() const { return m_dDuration; }
	const _double GetTimeAcc() const { return m_dTimeAcc; }
	const _float GetTotalAnimFastValue() const { return m_fTotalAnimationFastValue; }
	const _double GetAnimationProgressRate() const { return m_dTimeAcc / m_dDuration; }
	const _bool IsApplyRootBoneMove() const { return m_isApplyRootBoneMove; }

	void SetDuration(const _double _dDuration) { this->m_dDuration = _dDuration; }
protected:
	SHPTR<AAnimEvent> CreateAnimEvent(AAnimator* _pAnimator, ANIMEVENTTYPE _AnimEventType, std::ifstream& _read);
private:
	virtual void Free() override;
private:
	static constexpr _float		MAX_SUPPLY_VALUE{ 1.f };
	_double									m_dTickPerSeconds;
	_double									m_dDuration;
	_float										m_fTotalAnimationFastValue;
	_int											m_iAnimIndex;
	_string									m_strName;
	_bool										m_isApplyRootBoneMove;

	CHANNELCONTAINER			m_ChannelContainer;

	ATOMIC<_double>				m_dTimeAcc;
	_double									m_dAnimationProgressRate;
	_bool										m_isFinishAnimation;
	_bool										m_isSupplySituation;
	_float										m_fSupplySituationValue;

	ANIMFASTSECTIONS			m_AnimFastSectionContainer;
	ANIMEVENTCONTAINER		m_AnimEventContainer;
	SHPTR<AAnimEvent>			m_spActiveAnimChangeEvent;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_AANIMATION_H