#include "CoreDefines.h"
#include "AAnimation.h"
#include "AAnimator.h"
#include "AChannel.h"
#include "AAnimEvent.h"

namespace Core
{
	AAnimation::AAnimation() : 
	m_dTickPerSeconds{0}, m_dDuration{0}, m_fTotalAnimationFastValue{0}, m_iAnimIndex{0}, 
		m_strName{""}, m_isApplyRootBoneMove{false}, m_ChannelContainer{}, m_dTimeAcc{0},
		m_dAnimationProgressRate{ 0 }, m_isFinishAnimation{ false }, m_isSupplySituation{ false }, m_fSupplySituationValue{0},
		m_AnimFastSectionContainer{}, m_AnimEventContainer{}, m_spActiveAnimChangeEvent{nullptr}
	{
	}

	void AAnimation::Free()
	{
	}

	_bool AAnimation::NativeConstruct(AAnimator* _pAnimator, std::ifstream& _read)
	{
		ReadString(_read, m_strName);
		_read.read((_char*)&m_dTickPerSeconds, sizeof(_double));
		_read.read((_char*)&m_dDuration, sizeof(_double));
		_read.read((_char*)&m_fTotalAnimationFastValue, sizeof(_float));
		_read.read((_char*)&m_iAnimIndex, sizeof(_int));
		_read.read((_char*)&m_isApplyRootBoneMove, sizeof(_bool));

		size_t channelCnt = 0;
		_read.read((_char*)&channelCnt, sizeof(size_t));

		for (_int i = 0; i < channelCnt; ++i)
		{
			SHPTR<AChannel> spChannel = CreateInitNative<AChannel>(_pAnimator, _read);
			m_ChannelContainer.push_back(spChannel);
		}
		return true;
	}

	void AAnimation::UpdateBoneMatrices(const _double& _dTimeDelta)
	{
		_double dValue = m_dTickPerSeconds * _dTimeDelta;
		for (auto& iter : m_AnimFastSectionContainer)
		{
			iter.Convert(dValue, m_dTimeAcc);
		}
		m_dTimeAcc += (dValue * m_fTotalAnimationFastValue);

		if (m_dTimeAcc >= m_dDuration)
		{
			m_dTimeAcc = 0.0;
			m_dAnimationProgressRate = 0.f;
			m_isSupplySituation = false;
			m_isFinishAnimation = true;
			ResetAnimChangeEventNode();
		}
		else
		{
			for (auto& iter : m_ChannelContainer)
			{
				iter->UpdateTransformMatrix(m_dTimeAcc, this);
			}
			m_dAnimationProgressRate = m_dTimeAcc / m_dDuration;
			m_isFinishAnimation = false;
		}
	}

	void AAnimation::UpdateboneMatricesToRatio(const _double& _Ratio)
	{
		m_dTimeAcc = _Ratio;
		for (auto& iter : m_ChannelContainer)
		{
			iter->UpdateTransformMatrix(m_dTimeAcc, this);
		}
	}

	void AAnimation::UpdateNextAnimTransformMatrices(const _double& _dTimeDelta, const _float _fSupplyValue, CSHPTRREF<AAnimation> _spAnimation)
	{
		m_fSupplySituationValue += (_float)(_dTimeDelta * _fSupplyValue);

		if (m_fSupplySituationValue >= MAX_SUPPLY_VALUE)
		{
			m_fSupplySituationValue = 0.001f;
			m_isSupplySituation = false;
		}
		else
		{
			m_isSupplySituation = true;
			for (_uint i = 0; i < m_ChannelContainer.size(); ++i)
			{
				m_ChannelContainer[i]->UpdateSupplyToCurAndNextTransformMatrix(m_dTimeAcc, m_fSupplySituationValue,
					this, _spAnimation->m_ChannelContainer[i]);
			}
		}
	}

	void AAnimation::UpdateTimeAccToChannelIndex(const _double& _dTimeAcc)
	{
		m_dTimeAcc = _dTimeAcc;
		if (m_dTimeAcc >= m_dDuration)
		{
			m_dTimeAcc = m_dDuration;
		}
		for (auto& iter : m_ChannelContainer)
		{
			iter->ComputeCurKeyFrames(_dTimeAcc);
		}
	}

	void AAnimation::TickAnimEvent(APawn* _pPawn, AAnimator* _pAnimator, const _double& _TimeDelta, const _string& _strInputTrigger)
	{
		TickAnimEvent(_pPawn, _pAnimator, _TimeDelta, m_dTimeAcc, _strInputTrigger);
	}

	void AAnimation::TickAnimEvent(APawn* _pPawn, AAnimator* _pAnimator, const _double& _TimeDelta,
		const _double& _Ratio, const _string& _strInputTrigger)
	{
		// 만약 ActiveAnimChagneEvent가 활성화되지 않았다면, 활성화할때까지 찾아라
		if (nullptr == m_spActiveAnimChangeEvent)
		{
			for (auto& Event : m_AnimEventContainer[ANIMEVENTTYPE::ANIMEVENT_ANIMCHANGESBETWEEN])
			{
				if (true == Event->EventCheck(_pPawn, _pAnimator, _TimeDelta, _Ratio, _strInputTrigger))
				{
					m_spActiveAnimChangeEvent = Event;
					break;
				}
			}
		}
		else
		{
			// 이벤트 활성화
			m_spActiveAnimChangeEvent->EventCheck(_pPawn, _pAnimator, _TimeDelta, _Ratio, _strInputTrigger);
		}
		// Collider Event 
		for (auto& Event : m_AnimEventContainer[ANIMEVENTTYPE::ANIMEVENT_COLLIDER])
		{
			Event->EventCheck(_pPawn, _pAnimator, _TimeDelta, _Ratio, _strInputTrigger);
		}
		// Sound
		for (auto& Event : m_AnimEventContainer[ANIMEVENTTYPE::ANIMEVENT_SOUND])
		{
			Event->EventCheck(_pPawn, _pAnimator, _TimeDelta, _Ratio, _strInputTrigger);
		}
		// Effect
		for (auto& Event : m_AnimEventContainer[ANIMEVENTTYPE::ANIMEVENT_EFFECT])
		{
			Event->EventCheck(_pPawn, _pAnimator, _TimeDelta, _Ratio, _strInputTrigger);
		}
		// Camera
		for (auto& Event : m_AnimEventContainer[ANIMEVENTTYPE::ANIMEVENT_CAMERA])
		{
			Event->EventCheck(_pPawn, _pAnimator, _TimeDelta, _Ratio, _strInputTrigger);
		}
		// OBJ Active
		for (auto& Event : m_AnimEventContainer[ANIMEVENTTYPE::ANIMEVENT_OBJACTIVE])
		{
			Event->EventCheck(_pPawn, _pAnimator, _TimeDelta, _Ratio, _strInputTrigger);
		}
		// Collider
		for (auto& Event : m_AnimEventContainer[ANIMEVENTTYPE::ANIMEVENT_COLLIDER])
		{
			Event->EventCheck(_pPawn, _pAnimator, _TimeDelta, _Ratio, _strInputTrigger);
		}
	}

	void AAnimation::ResetData()
	{
	}

	void AAnimation::ResetAnimChangeEventNode()
	{
		m_spActiveAnimChangeEvent = nullptr;
	}

	void AAnimation::LoadAnimSectionData(const _string& _strFolderPath)
	{
		_string str = _strFolderPath;
		str.append("\\AnimSection\\");
		str.append(m_strName);
		str.append(".bin");

		std::ifstream Read{ str, std::ios::binary };
		RETURN_CHECK(!Read, ;);

		_uint iFastSection{ 0 };
		Read.read((_char*)&m_fTotalAnimationFastValue, sizeof(_float));
		Read.read((_char*)&iFastSection, sizeof(_uint));
		m_AnimFastSectionContainer.resize(iFastSection);
		Read.read((_char*)&m_AnimFastSectionContainer[0], sizeof(ANIMFASTSECTION) * iFastSection);
	}

	void AAnimation::LoadAnimEventData(AAnimator* _pAnimator, const _string& _strFolderPath)
	{
		_string str = _strFolderPath;
		str.append("\\AnimEvent\\");
		str.append(m_strName);
		str.append(".bin");

		std::ifstream Read{ str, std::ios::binary };
		RETURN_CHECK(!Read, ;);

		Read.read((_char*)&m_isApplyRootBoneMove, sizeof(_bool));
		// Anim Event Load
		{
			_uint iSize{ 0 };
			// Container Size Save 
			{
				Read.read((_char*)&iSize, sizeof(_uint));
			}
			for (_uint i = 0; i < iSize; ++i)
			{
				VECTOR<SHPTR<AAnimEvent>> AnimEventContainer;
				ANIMEVENTTYPE EventType;
				// Evnet Type Save
				{
					Read.read((_char*)&EventType, sizeof(ANIMEVENTTYPE));
				}
				_uint EventCnt{ 0 };
				AnimEventContainer.reserve(EventCnt);
				// Event Cnt Save
				{
					Read.read((_char*)&EventCnt, sizeof(_uint));
				}
				// Event save
				for (_uint j = 0; j < EventCnt; ++j)
				{
					AnimEventContainer.push_back(CreateAnimEvent(_pAnimator, EventType, Read));
				}
				m_AnimEventContainer.emplace(MakePair(EventType, AnimEventContainer));
			}
		}
	}

	SHPTR<AAnimEvent> AAnimation::CreateAnimEvent(AAnimator* _pAnimator, ANIMEVENTTYPE _AnimEventType, std::ifstream& _read)
	{
		SHPTR<AAnimEvent> spAnimEvent{ nullptr };
		switch (_AnimEventType)
		{
		case ANIMEVENT_CAMERA:
			break;
		case ANIMEVENT_ANIMCHANGESBETWEEN:
			spAnimEvent = Create<AAnimChangeBetweenEvent>(_pAnimator, _read);
			break;
		case ANIMEVENT_COLLIDER:
			spAnimEvent = Create<AAnimColliderEvent>(_pAnimator, _read);
			break;
		}
		return std::move(spAnimEvent);
	}

}