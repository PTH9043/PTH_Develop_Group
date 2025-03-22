#pragma once
#include "UBase.h"

BEGIN(Engine)
class UAnimModel;
class UAnimChannel;
class UAnimEvent;
class UPawn;
/*
@ Date: 2024-02-17, Writer: 박태현
@ Explain
- Animation를 담는 클래스 Channel들의 VECTOR 컨테이너를 들고 있다. 
*/
class   UAnimation : public UBase{
	using CHANNELS = VECTOR<SHPTR<UAnimChannel>>;
	using ANIMFASTSECTIONS = VECTOR<ANIMFASTSECTION>;
	using ANIMEVENTCONTAINER = UNORMAP<ANIMEVENTTYPE, VECTOR<SHPTR<UAnimEvent>>>;
public:
	UAnimation();
	UAnimation(const UAnimation& _rhs);
	DESTRUCTOR(UAnimation)
public:
	const _wstring& GetAnimName() const { return m_wstrName; }
	const _bool IsFinishAnim() const { return m_isFinishAnimation; }
	const _bool IsSupplySituation() const { return m_isSupplySituation; }
	const _double& GetDuration() const { return m_dDuration; }
	const _double& GetTimeAcc() const { return m_dTimeAcc; }
	const _float& GetTotalAnimFastValue() const { return m_fTotalAnimationFastValue; }
	const _double& GetAnimationProgressRate() const { return m_dAnimationProgressRate; }
	const VECTOR<ANIMFASTSECTION>& GetAnimFastSection() const { return m_AnimFastSections; }
	const ANIMEVENTCONTAINER& GetAnimEventContainer() const { return m_AnimEventContainer; }
	const _bool IsApplyRootBoneMove() const { return m_isApplyRootBoneMove; }
	const _bool IsInitializeAnimationMovement() const { return 0 == m_dTimeAcc; }
	// Set Finish
	void SetSupplySituation(const _bool _isSupplySituation) { this->m_isSupplySituation = _isSupplySituation; }
	void SetAnimTimeAcc(const _double& _dTimeAcc) { this->m_dTimeAcc = _dTimeAcc; }
	void UpdateAnimFastSections(const _float _fTotalAnimFastValue, const VECTOR<ANIMFASTSECTION>& _AnimFastSection);
	void SetApplyRootBoneMove(const _bool _isApplyRootBoneMove) { this->m_isApplyRootBoneMove = _isApplyRootBoneMove; }

	void UpdateTimeAccToChannelIndex(const _double& _dTimeAcc);

	void SetAnimIndex(_int _newIndex) { m_iAnimIndex = _newIndex; }
	
public:
	SHPTR<UAnimation> Clone(CSHPTRREF<UAnimModel> _spAnimModel);
	virtual void Free() override;
	HRESULT NativeConstruct(CSHPTRREF<UAnimModel> _spAnimModel, const ANIMDESC& _stAnimDesc, _int _AnimIndex);
	// 애니메이션과 연결된 뼈 정보들을 업데이트
	void UpdateBoneMatrices(const _double& _dTimeDelta);
	// TimeAcc로 애니메이션과 연결된 뼈 정보들을 업데이트
	void UpdateboneMatricesToRatio( const _double& _Ratio);
	// 다음 애니메이션으로 변경
	void UpdateNextAnimTransformMatrices(const _double& _dTimeDelta, const _float _fSupplyValue, CSHPTRREF<UAnimation> _spAnimation);
	// Animation Event Tick 
	void TickAnimEvent(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _TimeDelta, const _wstring& _wstrInputTrigger);
	void TickAnimEvent(UPawn* _pPawn, UAnimModel* _pAnimModel, const _double& _TimeDelta,  const _double& _Ratio, const _wstring& _wstrInputTrigger);
	void ResetData();
	// 애니메이션 이벤트를 집어넣는 함수
	void InsertAnimEvent(ANIMEVENTTYPE _AnimEventType, CSHPTRREF<UAnimEvent> _spAnimEvent);
	// 애니메이션 이벤트를 제거하는 함수
	void RemoveAnimEvent(CSHPTRREF<UAnimEvent> _spAnimEvent);
	// Remove Event
	void RemoveAnimEvent(ANIMEVENTTYPE _AnimEventType, _int _RemoveEvent);
	// 애니메이션 이벤트 노드를 제거
	void ResetAnimChangeEventNode() { m_spActiveAnimChangeEvent = nullptr; }
	// Save Sections
	void SaveAnimSectionData(const _wstring& _wstrPath);
	void SaveAnimSectionPathIsFolder(const _wstring& _wstrPath);
	void LoadAnimSectionData(const _wstring& _wstrPath);
	void LoadAnimSectionDataPathIsFolder(const _wstring& _wstrPath);
	// Save AnimEvent 
	void SaveAnimEventData(const _wstring& _wstrPath);
	void SaveAnimEventPathIsFolder(const _wstring& _wstrPath);
	void LoadAnimEventData(CSHPTRREF<UAnimModel> _spAnimModel, const _wstring& _wstrPath);
	void LoadAnimEventDataPathIsFolder(CSHPTRREF<UAnimModel> _spAnimModel, const _wstring& _wstrPath);

	void CopyAnimEvent(ANIMEVENTTYPE _eType , CSHPTRREF<UAnimation> _spAnimation);

	void SaveAnimToServerData(std::ofstream& _save);
private:
	SHPTR<UAnimEvent> CreateAnimEvent(CSHPTRREF<UAnimModel> _spAnimModel, ANIMEVENTTYPE _AnimEventType, std::ifstream& _read);
private:
	static constexpr _float	MAX_SUPPLY_VALUE{1.f};

	CHANNELS							m_Channels;
	_wstring									m_wstrName;
	_uint										m_iNumChannels;
	_double									m_dTickPerSeconds;
	_double									m_dDuration;
	_double									m_dTimeAcc;
	_bool										m_isFinishAnimation;
	_bool										m_isSupplySituation;
	_float										m_fSupplySituationValue;
	ANIMFASTSECTIONS			m_AnimFastSections;
	_float										m_fTotalAnimationFastValue;
	_double									m_dAnimationProgressRate;
	_int											m_iAnimIndex;
	// Animation Event 
	ANIMEVENTCONTAINER		m_AnimEventContainer;
	SHPTR< UAnimEvent>		m_spActiveAnimChangeEvent;
	// IS ApplyRootBone 
	_bool										m_isApplyRootBoneMove;
};

END