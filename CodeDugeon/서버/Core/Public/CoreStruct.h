#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_CORESTRUCT_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_CORESTRUCT_H

namespace Core {

	/*
	@ Date: 2024-04-07, Writer: 박태현
	@ Explain: 공간에 대한 정의 
	*/
	struct SPACEINFO {
		Vector3		vCenter;
		Vector3		vExtents;
		_int				Depths;
		_int				Index;

		SPACEINFO() : vCenter{}, vExtents{}, Depths{ 0 }, Index{ 0 } {}
		SPACEINFO(const Vector3& _vCenter, const Vector3& _vExtents, _int _Depths, _int _SpaceIndex = 0) :
			vCenter{ _vCenter }, vExtents{ _vExtents }, Depths{ _Depths }, Index{ _SpaceIndex } {}
	};
	/*
	@ Date: 2024-06-15, Writer: 박태현
	@ Explain: 콜라이더에 대한 정보 정의 
	*/
	struct COLLIDERINFO {
		_int							iColliderType;
		Vector3					vPos;
		Vector3					vScale;
	};
	/*
	@ Date: 2024-06-16, Writer: 박태현
	@ Explain: 캐릭터 스테이터스에 대한 정보 
	*/
	struct CHARSTATUS
	{
		_float						 fPower;
		_float						 fDefensive;
		ATOMIC<_float>	  fHp;
		_float						  fSaveHp;
		CHARSTATUS() : fPower{ 0.0f }, fDefensive{ 0.0f }, fHp{ 0.0f }, fSaveHp{ 0 }  {}
		CHARSTATUS(_float _fPower, _float _fDefensive, _float _fHp) : fPower{ _fPower }, fDefensive{ _fDefensive }, fHp{ _fHp }, fSaveHp{ fHp } {}
	};

#pragma region ANIMMATIONFASTSECTION 

	typedef struct tagAnimFastSection
	{
		_float fStartSpot = -1.f;
		_float fEndSpot = -1.f;
		_float fFastValue = 1.f;

		void Convert(_double& _dValue, const _double& _dTimeAcc)
		{
			if (fStartSpot <= _dTimeAcc && _dTimeAcc <= fEndSpot)
				_dValue *= fFastValue;
		}
		_bool IsPass(const _double& _dTimeValue)
		{
			if (fStartSpot <= _dTimeValue && _dTimeValue <= fEndSpot)
				return true;

			return false;
		}
	}ANIMFASTSECTION;

#pragma endregion ANIMATIONFASTSECTION


#pragma region ANIMEVENTTYPE


	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	- 애니메이션 이벤트 구조체의 상위 구조체
	*/
	struct ANIMEVENTDESC abstract {
	public:
		_bool							isActiveEvent;
		_string						strEventTrigger;
		_bool							isAnimChangeActive;

		ANIMEVENTDESC() : strEventTrigger{ "" }, isActiveEvent{ false }, isAnimChangeActive{ false }
		{
			strEventTrigger.resize(MAX_BUFFER_LENGTH);
		}
	};
	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	- 애니메이션 Event를 위한 구간을 정의하는 구조체
	- 애니메이션 Event와 ChangeDesc를 상속 받아서 그 값들을 사용한다.
	*/
	struct ANIMEVENTSECTIONDESC : public ANIMEVENTDESC {
		_double dStartTime;
		_double dEndTime;
		_double dStopTime;

		ANIMEVENTSECTIONDESC() : dStartTime{ 0.0 }, dEndTime{ 0.0 }, dStopTime{ 0.0 } {}

		_bool IsAnimEventActive(const _double& _dTimeAcc) const
		{
			if (_dTimeAcc >= dStartTime && _dTimeAcc <= dEndTime)
			{
				return true;
			}
			return false;
		}
	};
	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	- 애니메이션 Event가 발생된 지점을 정의하는 구조체
	- 애니메이션 Event와 ChangeDesc를 상속 받아서 그 값들을 사용한다.
	*/
	struct ANIMOCURRESDESC : public ANIMEVENTDESC {
		_double		dAnimOccursTime;

		ANIMOCURRESDESC() : dAnimOccursTime{ 0.0 } {}

		bool IsAnimOcurrs(const _double& _dTimeAcc) const{
			if (_dTimeAcc >= dAnimOccursTime)
				return true;
			return false;
		}
	};
	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	-  애니메이션의 Event Desc 말고 각자의 Other Event를 담고 있는 구조체로
		해당 구조체를 정의함으로써, 다른 POINTER들이 개입할 여지를 없애기 위해서 정의함
	*/
	struct ANIMOTHEREVENTDESC abstract { };

	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	-  애니메이션이 다음으로 넘어가기 위한 구조체
	*/
	struct ANIMCHANGEDESC : public ANIMOTHEREVENTDESC {
		_int				iNextAnimIndex;
		_float			fSupplyAnimValue;
		_double		dNextAnimTimeAcc;
		_float			fAnimChangeTime;
		_float			fLastProgressValue;
		_bool			isEnableLastSettingAnim;

		ANIMCHANGEDESC() : iNextAnimIndex{ 0 }, fSupplyAnimValue{ 1.f }, dNextAnimTimeAcc{ 0.0 },
			fAnimChangeTime{ 0 }, fLastProgressValue{ 1.f }, isEnableLastSettingAnim{ false }
		{}
		ANIMCHANGEDESC(const _int _NextAnimIndex, const _float& _SupplyAnimValue, const _double& _dNextAnimTimeAcc) :
			iNextAnimIndex{ _NextAnimIndex }, fSupplyAnimValue{ _SupplyAnimValue }, dNextAnimTimeAcc{ _dNextAnimTimeAcc },
			fAnimChangeTime{ 0 }, fLastProgressValue{ 1.f }, isEnableLastSettingAnim{ false }
		{}
	};
	/*
	@ Date: 2024-02-21, Writer: 박태현
	@ Explain
	-  애니메이션 특정 구역에 Collider를 붙이기 위한 구조체이다.
	*/
	struct ANIMCOLLIDERDESC : public ANIMOTHEREVENTDESC {
		_int												iColliderType;

		SHPTR<class ACollider>			spCollider;
		SHPTR<class ABoneNode>	spBoneNode;

		Vector3										vColliderScale;
		Vector3										vColliderTranslation;

		ANIMCOLLIDERDESC() : iColliderType{ 0 },
			spCollider{ nullptr }, spBoneNode{ nullptr }, vColliderScale{ 1, 1, 1 }, vColliderTranslation{ 1, 1, 1 } {}
		ANIMCOLLIDERDESC(const _int _iColliderType, const Vector3& _vColliderScale, const Vector3& _vColliderTranslation) :
			iColliderType{ _iColliderType }, vColliderScale{ _vColliderScale }, vColliderTranslation{ _vColliderTranslation }, spBoneNode{ nullptr }, spCollider{ nullptr }
		{}
	};
	/*
	@ Date: 2024-02-21, Writer: 박태현
	@ Explain
	-  애니메이션 특정 구역에 Collider를 붙이기 위한 구조체이다.
	*/
	struct ANIMSOUNDDESC : public ANIMOTHEREVENTDESC {
		_string				strSoundName;
		Vector3				vSoundVelocity;
		_float					fMinSoundDistance;
		_float					fMaxSoundDistance;

		ANIMSOUNDDESC() : strSoundName{ "" }, vSoundVelocity{}, fMinSoundDistance{ 0.f }, fMaxSoundDistance{ 1.f } {}

	};

	// Key Frame
	typedef struct tagKeyFrame
	{
		_double		dTime{ 0.0 };

		Vector3	vScale;
		Vector4	vRotation;
		Vector4	vPosition;
	}KEYFRAME;

	struct TIMEREVENT {
		_llong					llObjID;
		_llong					llTargetID;
		TIMEPOINT		WakeUpTime;
		EVENT_TYPE eEventType;

		TIMEREVENT() : llObjID{ 0 }, llTargetID{ 0 }, WakeUpTime{ TIMEPOINT::min()}, eEventType{EV_END} {}
		TIMEREVENT(const _llong _llObjID, const _llong _llTargetID, EVENT_TYPE _eEventType)
			: llObjID{ _llObjID }, llTargetID{ _llTargetID }, WakeUpTime{ std::chrono::system_clock::now() }, eEventType{_eEventType}
		{}

		constexpr bool operator < (const TIMEREVENT& L) const
		{
			return (WakeUpTime > L.WakeUpTime);
		}
	};

	typedef struct  tagCusTimer
	{
		tagCusTimer() = default;
		tagCusTimer(const _float& _fStandardTime, std::memory_order _memoryOrder = std::memory_order_relaxed)
			: fStandardTime(_fStandardTime), MemoryOrder{_memoryOrder}
		{
			fTimer = 0.f;
			isPass = false;
		}

		_bool IsOver(const _double& _dTimeDelta)
		{
			if (fTimer.load(MemoryOrder) >= fStandardTime)
			{
				isPass.store(true, MemoryOrder);
				return true;
			}
			fTimer.fetch_add((_float)_dTimeDelta, MemoryOrder);
			return false;
		}

		_bool IsOver()
		{
			if (fTimer.load(MemoryOrder) >= fStandardTime)
			{
				return true;
			}
			return false;
		}

		void ResetTimer()
		{
			fTimer.store(0.f, MemoryOrder);
			isPass.store(false, MemoryOrder);
		}

		_bool IsPass() { return isPass; }
		void SetStandardTime(_float _StandardTime) { fStandardTime = _StandardTime; }
		// Timer
		std::atomic<_float>		fTimer = 0.f;
		_float								fStandardTime = 0.f;
		std::atomic<_bool>		isPass{ false };
	private:
		std::memory_order		MemoryOrder;
	}CUSTIMER;

#pragma endregion ANIMEVENTTYPE

	typedef struct tagNavDesc {
		_uint iCurIndex{ 0 };
	}NAVDESC;

	struct CellPathNode {
		SHPTR<class ACell> cell;
		_float cost;
		_float heuristic;
		_float totalCost;
		SHPTR<class ACell> parent;

		bool operator>(const CellPathNode& other) const {
			return totalCost > other.totalCost;
		}
	};

	struct PathFindingState {
		using PATHFINDER = std::priority_queue<CellPathNode, VECTOR<CellPathNode>, std::greater<CellPathNode>>;
		using COSTFAR = std::unordered_map<SHPTR<class ACell>, _float>;
		using CAMEFROM = std::unordered_map<SHPTR<class ACell>, SHPTR<class ACell>>;

		PATHFINDER openSet;
		COSTFAR costSoFar;
		CAMEFROM cameFrom;
		SHPTR<class ACell> endCell;
		_bool isPathFound = false;
		VECTOR<SHPTR<class ACell>> path;

		Vector3	vStartPos;
		Vector3  vEndPos;
	};

}
#endif // _SERVERFRAMEWORK_CORE_PUBLIC_CORESTRUCT_H