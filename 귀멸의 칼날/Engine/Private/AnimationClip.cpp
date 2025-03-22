#include "AnimationClip.h"
#include "Animation.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Sound_Manager.h"

CAnimationClip::CAnimationClip()
	:CBase()
{
}
CAnimationClip::CAnimationClip(const CAnimationClip& _rhs)
	: CBase(), m_EventList(_rhs.m_EventList), m_isLoop(_rhs.m_isLoop)
	, m_fSpeed(_rhs.m_fSpeed), m_pMainAnimation(_rhs.m_pMainAnimation)
	, m_pSubAnimation(_rhs.m_pSubAnimation)
{
	// 이름을 복사한다.
	m_strName = _rhs.m_strName;
}
const _string& CAnimationClip::Get_MainAnimationName()
{
	if (nullptr == m_pMainAnimation)
		return STR_NULL;
	return m_pMainAnimation->Get_Name();
}

const _string& CAnimationClip::Get_SubAnimationName()
{
	if (nullptr == m_pSubAnimation)
		return STR_NULL;
	return m_pSubAnimation->Get_Name();
}

_string CAnimationClip::Get_RootBoneName()
{
	if (nullptr == m_pMainAnimation)
		return STR_NULL;
	return m_pMainAnimation->Get_RootBoneName();
}

const _double& CAnimationClip::Get_Duration()
{
	return m_pMainAnimation->Get_Duration();
}

const _double& CAnimationClip::Get_TickPerSecond()
{
	return m_pMainAnimation->Get_TickPerSecond();
}

const _double& CAnimationClip::Get_TimeAcc()
{
	return m_pMainAnimation->Get_TimeAcc();
}

const _double& CAnimationClip::Get_EventPlayTime()
{
	if (0.0 == m_dEventPlayTime)
	{
		for (auto& event : m_EventList)
		{
			if (Engine::CAnimationClip::EVENTTYPE_BONETRANSFORM == event.tEventType)
			{
				// 총 비율을 구한다.
				_double dRatio = event.dEndRatio - event.dStartRatio;
				m_dEventPlayTime = dRatio * (Get_Duration() / Get_TickPerSecond());

				return m_dEventPlayTime;
			}
		}
	}

	return m_dEventPlayTime;
}

_matrix CAnimationClip::Get_RootBoneMatrix()
{
	return m_pMainAnimation->Get_RootBoneMatrix();
}

_vector CAnimationClip::Get_RootTransform()
{
	return m_pMainAnimation->Get_RootTransform();
}

void CAnimationClip::Set_Ratio(_double dRatio)
{
	if (nullptr == m_pMainAnimation)
		return;
	// Ratio 값을 TimeAcc로 전환한다.
	_double TimeAcc = dRatio * m_pMainAnimation->Get_Duration();
	// MainAnimation의 TimeAcc값을 변경한다.
	m_pMainAnimation->Set_TimeAcc(TimeAcc);
}

void CAnimationClip::Set_TimeAcc(_double dTimeAcc)
{
	if (nullptr == m_pMainAnimation)
		return;
	m_pMainAnimation->Set_TimeAcc(dTimeAcc);

	if (nullptr != m_pSubAnimation)
		m_pSubAnimation->Set_TimeAcc(dTimeAcc);
}

void CAnimationClip::Set_MainAnimation(CAnimation* _pAnimation)
{
	m_pMainAnimation = _pAnimation;

	if (nullptr != m_pSubAnimation)
		m_pMainAnimation->Reset_Data();
}

void CAnimationClip::Set_SubAnimation(CAnimation* _pAnimation)
{
	m_pSubAnimation = _pAnimation;

	if (nullptr != m_pSubAnimation)
		m_pSubAnimation->Reset_Data();
}

_bool CAnimationClip::Is_Finish()
{
	return m_pMainAnimation->Is_Finished();
}

_bool CAnimationClip::Is_RootBone()
{
	if (nullptr == m_pMainAnimation)
		return false;

	return m_pMainAnimation->Is_RootBone();
}

_float2 CAnimationClip::Get_TrailValues()
{
	return _float2(m_fTrailStartValue, m_fTrailEndValue);
}

_bool CAnimationClip::Is_SupplementSituation()
{
	return m_pMainAnimation->Is_SupplementSituation();
}

void CAnimationClip::Set_SupplementSituation(const _bool& _bSituation)
{
	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Set_SupplementSituation(_bSituation);
}

void CAnimationClip::Set_RootBone(const _bool& isRootBone)
{
	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Set_RootBone(isRootBone);
}

void CAnimationClip::Set_RootBoneName(const _string& _strRootBoneName)
{
	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Set_RootBoneName(_strRootBoneName);
}

void CAnimationClip::Set_Finish(const _bool& isFinish)
{
	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Set_Finished(isFinish);
}

void CAnimationClip::Reset_AnimationData()
{
	// 애니메이션들의 데이터를 초기화한다.
	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Reset_Data_ClipAnimation();

	if (nullptr != m_pSubAnimation)
		m_pSubAnimation->Reset_Data_ClipAnimation();
}

void CAnimationClip::Set_AnimationSpeed(const _float& fFastValue)
{
	m_fSpeed = fFastValue;

	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Set_FastValue(m_fSpeed);

	if (nullptr != m_pSubAnimation)
		m_pSubAnimation->Set_FastValue(m_fSpeed);
}

void CAnimationClip::Set_OwnerTransform(CTransform* _pOwnerTransform)
{
	// Transform을 넣는다.
	if (nullptr != _pOwnerTransform)
		m_pOwnerTransform = _pOwnerTransform;
}

_double CAnimationClip::Get_Ratio()
{
	return m_pMainAnimation->Get_Ratio();
}

_bool CAnimationClip::Compute_Distance()
{
	// 타겟과의 거리가 현재 애니메이션의 거리값보다 짧다면 TargetDistance를 거리값으로 사용한다.
	if (0.f == m_fTargetDistance)
		return false;

	return m_fTargetDistance < m_fMoveDistance;
}

HRESULT CAnimationClip::NativeConstruct()
{
	return S_OK;
}

HRESULT CAnimationClip::NativeConstruct_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CAnimationClip::NativeConstruct(CAnimation* pMainAnimation, CAnimation* pSubAnimation, const vector<ANIMEVENTDESC>& EventList, _string strName)
{
	if (nullptr != pMainAnimation)
		m_pMainAnimation = pMainAnimation;

	if (nullptr != pSubAnimation)
		m_pSubAnimation = pSubAnimation;

	if (0 != EventList.size())
		m_EventList = EventList;

	m_strName = strName;

	return S_OK;
}

void CAnimationClip::Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pMainAnimation)
		return;

	// 이벤트를 업데이트한다.
	Update_Event(_dTimeDelta);
}

void CAnimationClip::Late_Tick(const _double& _dTimeDelta)
{
}

/// <summary>
/// 초기화되어야하는 값들을 초기화합니다.
/// </summary>
void CAnimationClip::Initialize_Data()
{
	if (nullptr != m_pMainAnimation)
	{
		m_pMainAnimation->Set_TimeAcc(0.0);
		m_pMainAnimation->Update_TransformMatrices(0.0);
		m_pMainAnimation->Set_FastValue(m_fSpeed);
		m_pMainAnimation->Set_RootTransform(XMVectorZero());
		m_pMainAnimation->Reset_Data_ClipAnimation();
	}

	if (nullptr != m_pSubAnimation)
	{
		m_pSubAnimation->Set_TimeAcc(0.0);
		m_pSubAnimation->Update_SubTransformMatrices(0.0);
		m_pSubAnimation->Set_FastValue(m_fSpeed);
		m_pSubAnimation->Set_RootTransform(XMVectorZero());
		m_pSubAnimation->Reset_Data_ClipAnimation();
	}

	// 혹시라도 중력이 꺼져있다면 다시 켜준다.
	m_bIsGravity = true;
	// 혹시라도 메쉬가 꺼져있다면 다시 켜준다.
	m_bIsMeshActive = true;
	// 이벤트 플레이 변수를 false로 만든다.
	m_bIsEventPlay = false;
	// 본 트랜스폼값을 false로 만든다.
	m_bIsBoneTransform = false;

	// 정보들을 초기화한다.
	// MoveDistance 값이 있었다면 0으로 바꿔준다.
	m_fTargetDistance = 0.f;
	// RootTransform을 0으로 준다.
	m_vColliderDir = _float4(0.f, 0.f, 0.f, 0.f);
	m_vColliderPos = _float4(0.f, 0.f, 0.f, 0.f);
	m_vColliderScale = _float4(0.f, 0.f, 0.f, 0.f);
	m_bIsColliderActive = false;
	m_bIsTrailActive = false;
	m_fHeight = 0.f;
	m_fPushedForce = 0.f;
	m_fMoveDistance = 0.f;
	m_fTrailStartValue = 0.f;
	m_fTrailEndValue = 0.f;


	m_bIsRandomPlay = false;
	for (auto& event : m_EventList)
		event.bIsFinish = false;
}

// 다음 애니메이션 클립의 메인 애니메이션과 선형보간한다.
void CAnimationClip::Update_NextAnimation(const _double& _dTimeDelta, CAnimationClip* _pNextClip)
{
	m_pMainAnimation->Updatet_NextAnim_TransformMatrix(_dTimeDelta, _pNextClip->m_pMainAnimation, m_bSupplementAnimation);
}

void CAnimationClip::Update_NextAnimation(const _double& _dTimeDelta, const _double& _dNextRatio, const _double& _dLerpSpeed, CAnimationClip* _pNextClip)
{
	// 다음 애니메이션의 timeAcc값을 구한다.
	//_double timeAcc = _dNextRatio * _pNextClip->Get_Duration();

	_pNextClip->Set_Ratio(_dNextRatio);

	_double timeAcc = _pNextClip->Get_TimeAcc();

	// 2인자에 선형보간 할 키프레임을 계산할 수 있는 TimeAcc값이 들어간다.
	m_pMainAnimation->Update_NextAnimClip_TransformMatrix(_dTimeDelta, timeAcc, _dLerpSpeed, _pNextClip->m_pMainAnimation, m_bSupplementAnimation);
}

void CAnimationClip::Update_TransformMatrices(const _double& _dTimeDelta)
{
	if (nullptr == m_pMainAnimation)
		return;

	// 메인 애니메이션의 정점 정보들을 업데이트한다.
	m_pMainAnimation->Update_TransformMatrices(_dTimeDelta, m_isLoop);

	// 만약 서브애니메이션 ( 얼굴 표정 )등의 애니메이션이 존재한다면 업데이트를 한다.
	if (nullptr != m_pSubAnimation)
	{
		// SubAnimation은 키프레임값이 없는것들만 업데이트를 해준다.
		m_pSubAnimation->Update_SubTransformMatrices(_dTimeDelta);
	}
}

void CAnimationClip::Update_TransformMatrices()
{
	if (nullptr == m_pMainAnimation)
		return;

	m_pMainAnimation->Update_TransformMatrices();
}

CHierarchyNode* CAnimationClip::Find_HierarchyNode(const _string& _strBoneName)
{
	if (0 != m_HierarchyNodeList.size())
	{
		for (auto& pNode : m_HierarchyNodeList)
		{
			// 노드 리스트에서 이름이 같은 노드를 찾으면 리턴한다.
			if (pNode->Get_Name() == _strBoneName)
				return pNode;
		}
	}

	// 못찾으면 nullptr 리턴
	return nullptr;
}

void CAnimationClip::Update_Event(const _double& _dTimeDelta)
{
	// 사운드를 랜덤으로 나오게 하기위한 리스트
	vector<CAnimationClip::ANIMEVENTDESC*> DefaultSoundList;
	vector<CAnimationClip::ANIMEVENTDESC*> RandomSoundList;

	for (auto& event : m_EventList)
	{
		if (EVENTTYPE_SOUND == event.tEventType)
		{
			if (0 == event.fAddValue)
				DefaultSoundList.push_back(&event);
			else
				RandomSoundList.push_back(&event);
		}
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if(0 != RandomSoundList.size())
		m_iRandomSoundIndex = pGameInstance->Get_RandomCount() % RandomSoundList.size();

	RELEASE_INSTANCE(CGameInstance);

	

	if (0 != m_EventList.size())
	{
		for (auto& event : m_EventList)
		{
			switch (event.tEventType)
			{
				//EFFECT
			case Engine::CAnimationClip::EVENTTYPE_EFFECT:
				Update_Event_Effect(event, _dTimeDelta);
				break;

				// 콜라이더 이벤트
			case Engine::CAnimationClip::EVENTTYPE_COLLIDER:
				Update_Event_Collider(event, _dTimeDelta);
				break;

				// UI
			case Engine::CAnimationClip::EVENTTYPE_UI:
				break;

				// 사운드
			//case Engine::CAnimationClip::EVENTTYPE_SOUND:
			//	Update_Event_Sound(event, _dTimeDelta);
			//	break;

				// 카메라
			case Engine::CAnimationClip::EVENTTYPE_CAMERA:
				break;

				// 모델 숨기기 ( 메쉬 하이드 )
			case Engine::CAnimationClip::EVENTTYPE_MESH:
				Update_Event_MeshHide(event, _dTimeDelta);
				break;

				// 무기가 붙어있는 뼈를 변경하는 이벤트
			case Engine::CAnimationClip::EVENTTYPE_WEAPONBONE:
				Update_Event_WeaponBone(event, _dTimeDelta);
				break;

				// 속도제어 이벤트의 업데이트를 돌린다.
			case Engine::CAnimationClip::EVENTTYPE_SPEEDSECTION:
				Update_Event_SpeedSection(event, _dTimeDelta);
				break;

				// RootTransform 이벤트의 업데이트를 돌린다.
			case Engine::CAnimationClip::EVENTTYPE_BONETRANSFORM:
				Update_Event_RootTransform(event, _dTimeDelta);
				break;

				// 중력 적용을 결정하는 이벤트
			case Engine::CAnimationClip::EVENTTYPE_GRAVITY:
				Update_Event_Gravity(event, _dTimeDelta);
				break;

				// 특정 시점에서의 이벤트 활성화 여부등을 체크하는 이벤트
			case Engine::CAnimationClip::EVENTTYPE_TIMECHECK:
				Update_Event_TimeCheck(event, _dTimeDelta);
				break;

				// 쓰고싶을때 사용하라고 만든 이벤트
			case Engine::CAnimationClip::EVENTTYPE_TRAIL:
				Update_Event_Trail(event, _dTimeDelta);
				break;
			}
		}				

		for(auto& event : DefaultSoundList)
			Update_Event_Sound(*event, _dTimeDelta);

		if (0 != RandomSoundList.size())
		{
			if (false == m_bIsRandomPlay)
			{
				Update_Event_Sound(*RandomSoundList[m_iRandomSoundIndex], _dTimeDelta);
				if (true == RandomSoundList[m_iRandomSoundIndex]->bIsFinish)
					m_bIsRandomPlay = true;
			}
		}
	}
}

void CAnimationClip::Update_Event_Collider(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// EndRatio가 넘어가면 bIsActive를 true로 한다.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			_event.bIsFinish = true;
			// 콜라이더의 활성화를 false로 한다.
			m_bIsColliderActive = false;
			// 콜라이더가 비활성화 되면 멤버변수로 갖고있는 콜라이더 타입 변수도 지워준다.
			m_strColliderType = "";
			// 비활성화되면 Dir을 0으로 만들어준다.
			//m_vColliderDir = _float4(0.f, 0.f, 0.f, 0.f);
			//m_vColliderPos = _float4(0.f, 0.f, 0.f, 0.f);
			//m_vColliderScale = _float4(1.f, 1.f, 1.f, 0.f);

			m_fPushedForce = 0.f;
			m_fHeight = 0.f;
		}
	}

	// 시작Ratio보다 크고 EndRatio보다 작을때만한다.  
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// 콜라이더 이벤트가 들어오면 콜라이더에게 활성화 여부를 넘기고 PushedForce를 지정한다.
			m_bIsColliderActive = true;

			// 콜라이더의 종류
			m_strColliderType = _event.strEventText;
			// 콜라이더의 방향값
			m_vColliderDir = _event.vEventDir;
			// 콜라이더 위치
			m_vColliderPos = _event.vEventPos;
			// 콜라이더 크기
			m_vColliderScale = _event.vEventValues;
			// 밀리는 힘
			m_fPushedForce = _event.fValue;
			// Y값 ( 포물선 )
			m_fHeight = _event.fAddValue;
		}
	}
}

void CAnimationClip::Update_Event_Sound(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			_event.bIsFinish = true;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			{
				if (nullptr != pGameInstance)
				{
					if(1 == _event.vEventValues.x)
						// 사운드를 재생시킨다.
						pGameInstance->Play_SoundW(CMethod::ConvertSToW(_event.strEventText).c_str(), _event.fValue);
					else
						pGameInstance->Play_Sound_ByCondition(CMethod::ConvertSToW(_event.strEventText).c_str(), _event.fValue);
				}
			}
			RELEASE_INSTANCE(CGameInstance);
		}
	}
}

void CAnimationClip::Update_Event_Effect(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio()/* && m_pMainAnimation->Get_Ratio() <= _event.dEndRatio*/)
	{
		// 아직 활성화 되지 않았었다면 활성화 해주며, 애니메이션의 속도를 바꿔준다.
		if (false == _event.bIsFinish)
		{
			// 활성화
			_event.bIsFinish = true;
			// Bone 이름으로 하이어라키 노드를 찾아온다.
			CHierarchyNode* pNode = Find_HierarchyNode(_event.strEventText);
			if (nullptr == pNode)
				return;

			// 이곳 아래에서 얻어온 pNode를 Effect 부르셔서 넣어주시면 됩니다.
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			_float3 vLook = m_vRootBoneLook;
			_float3 vDir = m_vOwnerDirection;

			CGameObject* pEffect = pGameInstance->Active_Object(CMethod::ConvertSToW(_event.strName).c_str());
			if (nullptr != pEffect)
			{
				static_cast<CEffectObject*>(pEffect)->SetUP_Matrix(pNode, m_pOwnerTransform, _event.fValue, _event.vEventValues, m_vOwnerDirection, m_vRootBoneLook);
				static_cast<CEffectObject*>(pEffect)->Set_Play(true);
			}

			RELEASE_INSTANCE(CGameInstance);
		}
	}
	/*else if (m_pMainAnimation->Get_Ratio() >= _event.dEndRatio)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pEffect = pGameInstance->Active_Object(CMethod::ConvertSToW(_event.strName).c_str());
		if (nullptr != pEffect)
		{
			static_cast<CEffectObject*>(pEffect)->EffectObjectReset();
			static_cast<CEffectObject*>(pEffect)->Set_Active(false);
		}

		RELEASE_INSTANCE(CGameInstance);
	}*/
}

void CAnimationClip::Update_Event_SpeedSection(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// 시간이 없다면 EndRatio로따지고 아니라면 시간 비율로 따진다.

	// 이벤트가 끝나는 진행도값보다 현재 애니메이션의 진행도가 높다면 애니메이션들의 속도를 원래대로 돌려준다.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// 애니메이션 속도를 설정해준다.
			_event.bIsFinish = true;
			m_pMainAnimation->Set_FastValue(m_fSpeed);
			// 서브 애니메이션이 nullptr이 아니라면 서브애니메이션의 속도 또한 조절해준다.
			if (nullptr != m_pSubAnimation)
				m_pSubAnimation->Set_FastValue(m_fSpeed);
		}
	}
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		// 아직 활성화 되지 않았었다면 활성화 해주며, 애니메이션의 속도를 바꿔준다.
		if (false == _event.bIsFinish)
		{
			// 애니메이션 속도를 설정해준다.
			m_pMainAnimation->Set_FastValue(_event.fValue);
			// 서브 애니메이션이 nullptr이 아니라면 서브애니메이션의 속도 또한 조절해준다.
			if (nullptr != m_pSubAnimation)
				m_pSubAnimation->Set_FastValue(_event.fValue);
		}
	}
}

void CAnimationClip::Update_Event_MeshHide(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// 이벤트가 끝나는 진행도값보다 현재 애니메이션의 진행도가 높다면 중력을 다시 true로 해준다.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// 이벤트가 끝났다고 체크한다.
			_event.bIsFinish = true;
			// 중력값을 다시 적용한다.
			m_bIsMeshActive = true;
		}
	}
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		// 아직 활성화 되지 않았었다면 활성화 해주며, 애니메이션의 속도를 바꿔준다.
		if (false == _event.bIsFinish)
		{
			// 중력 적용을 끈다.
			m_bIsMeshActive = false;
		}
	}
}

void CAnimationClip::Update_Event_WeaponBone(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		// 아직 활성화 되지 않았었다면 활성화 해주며, 뼈 인덱스 번호를 바꾼다.
		if (false == _event.bIsFinish)
		{
			// 활성화
			_event.bIsFinish = true;
			// 애니메이션 속도를 설정해준다.
			m_iWeaponBoneIndex = (_uint)_event.fValue;
		}
	}
}

void CAnimationClip::Update_Event_RootTransform(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// 이벤트가 끝나는 진행도값보다 현재 애니메이션의 진행도가 높다면 애니메이션들의 속도를 원래대로 돌려준다.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			m_bIsEventPlay = false;
			m_bIsBoneTransform = false;


			// MoveDistance 값이 있었다면 0으로 바꿔준다.
			m_fMoveDistance = 0.f;
			m_fTargetDistance = 0.f;
			_event.bIsFinish = true;
			// RootTransform을 0으로 준다.
			m_pMainAnimation->Set_RootTransform(XMVectorZero());
		}
	}

	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		// 아직 끝나지 않았다면 매 프레임마다 Root의 이동값을 넣어준다
		if (false == _event.bIsFinish)
		{
			m_bIsEventPlay = true;
			m_bIsBoneTransform = true;

			m_fMoveDistance = _event.fValue;

			// 총 비율을 구한다.
			_double dRatio = _event.dEndRatio - _event.dStartRatio;
			m_dEventPlayTime = dRatio * (Get_Duration() / Get_TickPerSecond());
			// 남은 플레이타임을 구한다.
			_double dRestRatio = _event.dEndRatio - Get_Ratio();
			m_dRestPlayTime = dRestRatio * (Get_Duration() / Get_TickPerSecond());

			if (nullptr == m_pOwnerTransform)
				return;

			// 주인 객체의 Right, Up, Look을 받아온다.
			_vector vRight = m_pOwnerTransform->Get_Right();
			_vector vUp = m_pOwnerTransform->Get_Up();
			_vector vLook = m_pOwnerTransform->Get_Look();
			// RootTransform 계산값을 얻어온다.
			_vector vRootTransform = XMVectorZero();

			// 거리값을 받아오면 fMoveDistance를 해당 거리값으로 조정한다.
			_float fMoveDistance = 0.f;

			if (true == Compute_Distance())
				fMoveDistance = m_fTargetDistance;
			else
				fMoveDistance = _event.fValue;

			if (_event.vEventPos.x != 0)
				vRootTransform += XMVector3Normalize(vRight) * _event.vEventPos.x * fMoveDistance / (_float)m_dEventPlayTime * _float(_dTimeDelta) * Get_AnimationSpeed();
			if (_event.vEventPos.y != 0)
				vRootTransform += XMVector3Normalize(vUp) * _event.vEventPos.y * fMoveDistance / (_float)m_dEventPlayTime * _float(_dTimeDelta) * Get_AnimationSpeed();
			if (_event.vEventPos.z != 0)
				vRootTransform += XMVector3Normalize(vLook) * _event.vEventPos.z * fMoveDistance / (_float)m_dEventPlayTime * _float(_dTimeDelta) * Get_AnimationSpeed();

			m_fBoneAnimMoveForce = _event.fValue;

			// 이벤트의 Y값이 존재하지 않았다면 RootTransform 계산값에서 Y를 뺀다.
			if (0 == _event.vEventPos.y)
				vRootTransform = XMVectorSetY(vRootTransform, 0.f);

			_float ratio = 0.8f;
			_float MaxHeight = _event.fAddValue;

			_float fCurTime = (_float)(Get_TimeAcc() / Get_TickPerSecond());

			_float Gravity = (_float)(2 * MaxHeight / pow(ratio * m_dEventPlayTime, 2.f));
			_float Height = (_float)(Gravity * m_dEventPlayTime * ratio);
			_float fY = (_float)(Height - Gravity * fCurTime) * _float(_dTimeDelta);
			if (0.f != MaxHeight)
				vRootTransform = XMVectorSetY(vRootTransform, fY);

			// Root의 이동값을 넣어준다.
			m_pMainAnimation->Set_RootTransform(vRootTransform);
		}
	}
}

void CAnimationClip::Update_Event_Gravity(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// 이벤트가 끝나는 진행도값보다 현재 애니메이션의 진행도가 높다면 중력을 다시 true로 해준다.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// 이벤트가 끝났다고 체크한다.
			_event.bIsFinish = true;
			// 중력값을 다시 적용한다.
			m_bIsGravity = true;
		}
	}
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		// 아직 활성화 되지 않았었다면 활성화 해주며, 애니메이션의 속도를 바꿔준다.
		if (false == _event.bIsFinish)
		{
			// 중력 적용을 끈다.
			m_bIsGravity = false;
		}
	}
}

void CAnimationClip::Update_Event_TimeCheck(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// 이벤트에서 설정한 EndRatio가 현재 진행중인 Ratio값보다 작아지면 이벤트를 끝낸다.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// 이벤트가 끝났다고 체크한다.

			if (true == m_tEventCheck.bIsActive && _event.strName == m_tEventCheck.strEventName)
			{
				_event.bIsFinish = true;
				m_tEventCheck.bIsActive = false;
			}
		}
	}

	// 이벤트에서 설정한 StartRatio가 현재 진행중인 Ratio값보다 작아지면 이벤트를 시작한다.
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// 조건이 충족되면 체크된 이벤트 정보를 담는다.
			if (false == m_tEventCheck.bIsActive)
			{
				m_tEventCheck.bIsActive = true;
				m_tEventCheck.strEventName = _event.strName;
			}
		}
	}
}

void CAnimationClip::Update_Event_Trail(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// 이벤트에서 설정한 EndRatio가 현재 진행중인 Ratio값보다 작아지면 이벤트를 끝낸다.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			_event.bIsFinish = true;

			m_bIsTrailActive = false;
			m_fTrailStartValue = 0.f;
			m_fTrailEndValue = 0.f;
		}
	}

	// 이벤트에서 설정한 StartRatio가 현재 진행중인 Ratio값보다 작아지면 이벤트를 시작한다.
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			m_bIsTrailActive = true;
			m_fTrailStartValue = _event.fValue;
			m_fTrailEndValue = _event.fAddValue;
		}
	}
}

CAnimationClip* CAnimationClip::Create(CAnimation* pMainAnimation, CAnimation* pSubAnimation, const vector<ANIMEVENTDESC>& EventList, _string strName)
{
	CAnimationClip* pInstance = new CAnimationClip();

	if (FAILED(pInstance->NativeConstruct(pMainAnimation, pSubAnimation, EventList, strName)))
	{
		MSG_BOX("Failed to Created : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimationClip* CAnimationClip::Create()
{
	CAnimationClip* pInstance = new CAnimationClip();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimationClip* CAnimationClip::Clone(void* pArg)
{
	CAnimationClip* pInstance = new CAnimationClip(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimationClip::Free()
{
	__super::Free();
}