#include "Animator.h"
#include "AnimationClip.h"
#include "Animation.h"
#include "Model.h"
#include "HierarchyNode.h"
#include "GameInstance.h"

CAnimator::CAnimator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CComponent(_pDevice, _pContext)
{
}

CAnimator::CAnimator(const CAnimator& _rhs)
	:CComponent(_rhs)
{
}

 _bool CAnimator::Is_RootBone()
{
	if (nullptr == m_pCurrentClip)
		 return false;

	return m_pCurrentClip->Is_RootBone();
}

 _bool CAnimator::Is_BoneTransform()
 {
	 if (nullptr == m_pCurrentClip)
		 return false;

	 return m_pCurrentClip->Is_BoneTransform();
 }

 _bool CAnimator::Is_Gravity()
 {
	 if (nullptr == m_pCurrentClip)
		 return false;

	 return m_pCurrentClip->Is_Gravity();
 }

 _bool CAnimator::Is_MeshActive()
 {
	 if (nullptr == m_pCurrentClip)
		 return false;

	 return m_pCurrentClip->Is_MeshActive();
 }

 _bool CAnimator::Is_TrailActive()
 {
	 if (nullptr == m_pCurrentClip)
		 return false;

	 return m_pCurrentClip->Is_TrailActive();
 }

 _float2 CAnimator::Get_TrailValues()
 {
	 if (nullptr == m_pCurrentClip)
		 return _float2(0.f, 0.f);

	 return m_pCurrentClip->Get_TrailValues();
 }

void CAnimator::Set_Transform(CTransform* pTransform)
{
	if (nullptr == pTransform)
		return;

	m_pOwnerTransformCom = pTransform;
}

void CAnimator::Set_Model(CModel* pModel)
{
	if (nullptr == pModel)
		return;

	// 전에 이미 들어있었다면 Safe_Release();
	Safe_Release(m_pModelCom);

	// 모델객체를 pModel로
	m_pModelCom = pModel;

	// 모델의 레퍼런스를 증가시킨다.
	Safe_AddRef(pModel);

	// 모델의 뼈를 한번 초기화해준다.
	Update_InitBoneState();
}

CAnimationClip* CAnimator::Get_NextAnimationClip()
{
	if (nullptr == m_pNextClip)
		return nullptr;

	return m_pNextClip;
}

CAnimationClip* CAnimator::Get_AnimationClip(_string _strName)
{
	// 이름과 같은 애니메이션 클립을 찾았다면 해당 애니메이션 클립을 리턴해준다.
	if (0 != m_AnimationClips.size())
	{
		for (auto& pClip : m_AnimationClips)
		{
			if (_strName == pClip->Get_Name())
				return pClip;			
		}
	}
	return nullptr;
}

_string CAnimator::Get_CurrentTrigger()
{
	if (nullptr != m_pCurTransition)
		return m_pCurTransition->InputTrigger.strInput;
	else
		return _string("");
}

_float CAnimator::Get_MoveDistance()
{
	if (nullptr == m_pCurrentClip)
		return 0.f;

	// 애니메이션 이동값을 받아온다.
	return m_pCurrentClip->Get_MoveDistance();
}

_float	CAnimator::Get_MoveForce()
{
	_float fMoveForce;

	if (nullptr == m_pCurrentClip)
		fMoveForce = 0.f;
	else
		fMoveForce = m_pCurrentClip->Get_BoneMoveForce();

	return fMoveForce;
}

_vector CAnimator::Get_ColliderDir()
{
	if (nullptr == m_pCurrentClip)
		return XMVectorZero();

	_vector vDir = m_pCurrentClip->Get_ColliderDir();

	return m_pCurrentClip->Get_ColliderDir();
}

_vector CAnimator::Get_ColliderPos()
{
	if (nullptr == m_pCurrentClip)
		return XMVectorZero();

	_vector vPos = m_pCurrentClip->Get_ColliderPos();

	return m_pCurrentClip->Get_ColliderPos();
}

_float3 CAnimator::Get_RootBoneLook()
{
	return m_vRootBoneRotation;
}

_vector CAnimator::Get_ColliderScale()
{
	if (nullptr == m_pCurrentClip)
		return XMVectorZero();

	_vector vScale = m_pCurrentClip->Get_ColliderScale();

	return m_pCurrentClip->Get_ColliderScale();
}

_string CAnimator::Get_MainAnimationName()
{
	return m_pCurrentClip->Get_MainAnimationName();	
}

_string CAnimator::Get_SubAnimationName()
{
	return m_pCurrentClip->Get_SubAnimationName();
}

const _string& CAnimator::Get_AnimationType()
{
	// 만약 현재 클립이 보간중이라면 다음 바뀔 애니메이션이 있는지 확인 후 , 다음 애니메이션의 타입을 반환한다.
	if (true == m_pCurrentClip->Is_SupplementSituation())
	{
		if (nullptr != m_pNextClip)		
			return m_pNextClip->Get_AnimationType();		
	}
	return m_pCurrentClip->Get_AnimationType();
}

void CAnimator::Set_Stop()
{
	if (nullptr == m_pCurrentClip)
		return;

	m_bIsPlay = false;
	// 데이터를 초기화한다.
	m_pCurrentClip->Initialize_Data();
	Update_InitBoneState();
}

void CAnimator::Set_TimeAcc(_double dTimeAcc)
{
	if (nullptr == m_pCurrentClip)
		return;

	m_pCurrentClip->Set_TimeAcc(dTimeAcc);
}

void CAnimator::Set_CurrentAnimationClip(CAnimationClip* pClip)
{
	pClip->Initialize_Data();
	if(nullptr != m_pCurrentClip)
		m_pCurrentClip->Initialize_Data();
	m_pCurrentClip = pClip;
	if (nullptr != m_pNextClip)
		m_pNextClip = nullptr;
	Update_TransitionList();
	m_bIsPlay = true;
	m_vBoneTransformPos = _float4(0.f, 0.f, 0.f, 0.f);

	// 현재 클립이 OwnerTransform이 없다면 채워준다.
	if (nullptr == m_pCurrentClip->Get_OwnerTransform())
		return m_pCurrentClip->Set_OwnerTransform(m_pOwnerTransformCom);
	if (nullptr != m_pOwnerTransformCom)
		m_vBeginTransformMatrix = m_pOwnerTransformCom->Get_WorldFloat4x4();
}

void CAnimator::Set_CurrentAnimationClip(_string _strName)
{
	for (auto& pClip : m_AnimationClips)
	{
		// 이름이 같다면 해당 클립을 현재 클립으로 설정한다.
		if (_strName == pClip->Get_Name())
		{
			pClip->Initialize_Data();
			if (nullptr != m_pCurrentClip)
				m_pCurrentClip->Initialize_Data();			
			m_pCurrentClip = pClip;
			if (nullptr != m_pNextClip)
				m_pNextClip = nullptr;
			Update_TransitionList();
			m_bIsPlay = true;
			m_vBoneTransformPos = _float4(0.f, 0.f, 0.f, 0.f);

			// 현재 클립이 OwnerTransform이 없다면 채워준다.
			if (nullptr == m_pCurrentClip->Get_OwnerTransform())
				return m_pCurrentClip->Set_OwnerTransform(m_pOwnerTransformCom);
			// 트랜스폼 컴포넌트를 갖고있다면 주인 객체의 현재 위치를 받아온다.
			if (nullptr != m_pOwnerTransformCom)
				m_vBeginTransformMatrix = m_pOwnerTransformCom->Get_WorldFloat4x4();
		}
	}
}

void CAnimator::Set_NextDirectAnimationClip(CAnimationClip* pClip)
{
	m_pNextDirectClip = pClip;
}

void CAnimator::Set_NextDirectAnimationClip(const _string _strName)
{
	for (auto& pClip : m_AnimationClips)
	{
		// 이름이 같다면 해당 클립을 현재 클립으로 설정한다.
		if (_strName == pClip->Get_Name())
		{
			m_pNextDirectClip = pClip;	
			return;
		}
	}
}

void CAnimator::Set_AnimationClipToEntry()
{
	for (auto& transition : m_TransitionList)
	{
		// Entry 애니메이션이라면 현재 클립으로 설정해준다.
		if (ENTRY == transition.tEnterState)		
			Set_CurrentAnimationClip(transition.pExitClip);		
	}
}

vector<CHierarchyNode*> CAnimator::Get_WeaponBoneList()
{
	return m_WeaponBoneList;
}

CHierarchyNode* CAnimator::Get_WeaponBone()
{
	if (0 == m_WeaponBoneList.size())
		return nullptr;

	return m_WeaponBoneList[m_pCurrentClip->Get_WeaponBoneIndex()];
}

_double CAnimator::Get_Ratio()
{
	if (nullptr == m_pCurrentClip)
		return 0.0;

	return m_pCurrentClip->Get_Ratio();
}

_double CAnimator::Get_Duration()
{
	if (nullptr == m_pCurrentClip)
		return 0.0;
	return m_pCurrentClip->Get_Duration();
}

_double CAnimator::Get_TickPerSecond()
{
	if (nullptr == m_pCurrentClip)
		return 0.0;
	return m_pCurrentClip->Get_TickPerSecond();
}

_double CAnimator::Get_TimeAcc()
{
	if (nullptr == m_pCurrentClip)
		return 0.0;
	return m_pCurrentClip->Get_TimeAcc();
}

_float CAnimator::Get_AnimationSpeed()
{
	if (nullptr == m_pCurrentClip)
		return 0.0f;
	return m_pCurrentClip->Get_AnimationSpeed();
}

_matrix CAnimator::Get_RootBoneMatrix()
{
	return XMLoadFloat4x4(&m_vRootBoneMatrix);
}

const _bool& CAnimator::Is_AttackColliderActive()
{
	return m_pCurrentClip->Is_AttackColliderActive();
}

const _float& CAnimator::Get_PushedForce()
{
	return m_pCurrentClip->Get_PushedForce();
}

const _float& CAnimator::Get_Height()
{
	return m_pCurrentClip->Get_Height();
}

const _double& CAnimator::Get_EventPlayTime()
{
	return m_pCurrentClip->Get_EventPlayTime();
}

const _bool& CAnimator::Is_EventPlay()
{
	return m_pCurrentClip->Is_EventPlay();
}

void CAnimator::Play_OneFrame(_double TimeDelta)
{
	m_bIsPlay = false;

	// 뼈값을 업데이트한다.
	if (0 < Get_TimeAcc() + TimeDelta)
		Update_AnimationClip(TimeDelta);
	else if (0 >= Get_TimeAcc() + TimeDelta)
	{
		Set_TimeAcc(0.0);
		// 0.0으로 애니메이션 업데이트를 돌려서 뼈값만 설정해준다.
		m_pCurrentClip->Update_TransformMatrices(0.0);

		// 모델 객체의 뼈값을 업데이트한다.
		if (nullptr != m_pModelCom)
		{
			vector<CHierarchyNode*>vecHierachyNodes = m_pModelCom->Get_HierachyNodeList();
			/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
			for (auto& pHierarchyNode : vecHierachyNodes)
				pHierarchyNode->Update_CombinedTransformationMatrix();
		}
	}
}

void CAnimator::Set_Ratio(_double dRatio)
{
	if (nullptr == m_pCurrentClip)
		return;

	m_pCurrentClip->Set_Ratio(dRatio);
	// 애니메이션 진행도의 변경이 끝났으면 애니메이션 뼈값을 초기화한다.
		// 0.0으로 애니메이션 업데이트를 돌려서 뼈값만 설정해준다.
	m_pCurrentClip->Update_TransformMatrices(0.0);

	// 모델 객체의 뼈값을 업데이트한다.
	if (nullptr != m_pModelCom)
	{
		vector<CHierarchyNode*>vecHierachyNodes = m_pModelCom->Get_HierachyNodeList();
		/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
		for (auto& pHierarchyNode : vecHierachyNodes)
			pHierarchyNode->Update_CombinedTransformationMatrix();
	}
	//Update_InitBoneState();	
}

void CAnimator::Add_AnimationClip(CAnimationClip* _pClip)
{
	if (nullptr == _pClip)
		return;

	for (auto& pClip : m_AnimationClips)
	{
		if (pClip->Get_Name() == _pClip->Get_Name())
		{
			Safe_Release(_pClip);
			return;
		}
	}
	if(nullptr != m_pOwnerTransformCom)
		_pClip->Set_OwnerTransform(m_pOwnerTransformCom);

	// Clone된 객체들을 바로 삭제하기위해 Safe_Release등의 작업은 패스한다.
	m_AnimationClips.push_back(_pClip);
}

void CAnimator::Delete_AnimationClip(_string strName)
{
	for (int i = 0; i < m_AnimationClips.size(); ++i)
	{
		if (m_AnimationClips[i]->Get_Name() == strName)
		{
			// Safe_Release로 클립을 지워준다.
			Safe_Release(m_AnimationClips[i]);
			// 리스트에서 삭제한다
			m_AnimationClips.erase(m_AnimationClips.begin() + i);
		}
	}
}

void CAnimator::Make_Transition()
{
	TRANSITION transition;

	transition.tEnterState = DEFAULT;
	transition.pEnterClip = nullptr;
	transition.pExitClip = nullptr;
	
	// 트랜지션 리스트에 트랜지션을 추가한다.
	m_TransitionList.push_back(transition);
}


_bool CAnimator::Input_Trigger(_string _trigger)
{
	if (nullptr == m_pCurrentClip)
		return false;

	// 플레이중이 아닐때는 트리거 입력을 받지 말자
	if (false == m_bIsPlay)
		return false;

	INPUTTRIGGER inputTrigger;
	inputTrigger.dAnimRatio = m_pCurrentClip->Get_Ratio();
	inputTrigger.strInput = _trigger;

	// AnyState인 경우 ( 보통 피격의 경우 ) Exit가 같고 같은 Trigger를 가진 Default인 Transition이 존재할 경우 Default로 넘기기위해 담아준다.
	TRANSITION* pTransition = nullptr;

	// 받아온 트리거를 Transition에서 AnyState 에서 옮길 수 있는 클립인지 검색합니다.
	for (auto& Transition : m_ActiveTransitionList)
	{
		// 아무 상태에서나 갈 수 있는 클립중에 트리거가 같은 클립을 찾는다.
		if (ANYSTATE == Transition.tEnterState)
		{
			if (Transition.Compare_Trigger(inputTrigger))
			{
				// AnyState인 Transition을 담아준다.
				pTransition = &Transition;
				
				for (auto& findTransition : m_ActiveTransitionList)
				{
					// 트리거가 같은것중 Default 상태인것이 있다면 현재 재생중인 애니메이션과 해당 트랜지션의 EnterClip과 비교하여 모든 조건이 충족할경우 교체한다.
					if (findTransition.InputTrigger.strInput == pTransition->InputTrigger.strInput)
					{
						if (DEFAULT == findTransition.tEnterState)
						{
							if (m_pCurrentClip == findTransition.pEnterClip)
							{
								// 다음 애니메이션 클립을 지정해준다.
								m_pNextClip = findTransition.pExitClip;
								m_pCurTransition = &findTransition;
								return true;
							}
							continue;
						}						
						continue;
					}
					continue;
				}
				// 다음 애니메이션 클립을 지정해준다.
				m_pNextClip = Transition.pExitClip;
				m_pCurTransition = &Transition;
				return true;
			}
		}
		// AnyState가 아닌 경우에 모든 조건을 충족했다면 클립을 변경한다.
		else
		{
			//// 이미 바뀔 클립이 있을 경우에는 AnyState를 제외한 다른 트리거를 받지 않는다.
			//if (nullptr != m_pNextClip)
			//	return false;

			// 트리거의 조건을 만족한 경우
			if (Transition.Compare_Trigger(inputTrigger) && m_pNextClip != m_pCurrentClip)
			{
				// ChangeRatio값보다 비율이 높은경우에 작동한다면 체크 후 교체한다.
				if (true == Transition.bIsOverChangeRatio)
				{
					if (inputTrigger.dAnimRatio >= Transition.dChangeRatio)
					{
						// 다음 애니메이션 클립을 지정해준다.
						m_pNextClip = Transition.pExitClip;
						m_pCurTransition = &Transition;
						return true;
					}
					else
					{
						// 바뀔 애니메이션이 없으면 트리거리스트에 추가한다.
						// 같은 트리거가 있는지 검사하는 불값
						_bool isSame = false;
						for (auto& trigger : m_InputTriggerList)
						{
							if (trigger.strInput == inputTrigger.strInput)
							{
								trigger.dAnimRatio = inputTrigger.dAnimRatio;
								isSame = true;
							}
						}
						// 같은게 없다면 데이터를 넣어준다.
						if (isSame == false)
						{
							m_InputTriggerList.push_back(inputTrigger);
							return true;
						}
					}
				}
				// ChangeRatio값보다 비율이 작은경우에 작동한다면 체크 후 교체한다.
				else
				{
					if (inputTrigger.dAnimRatio <= Transition.dChangeRatio)
					{
						// 다음 애니메이션 클립을 지정해준다.
						m_pNextClip = Transition.pExitClip;
						m_pCurTransition = &Transition;
						return true;
					}
					else
					{
						// 바뀔 애니메이션이 없으면 트리거리스트에 추가한다.
						// 같은 트리거가 있는지 검사하는 불값
						_bool isSame = false;
						for (auto& trigger : m_InputTriggerList)
						{
							if (trigger.strInput == inputTrigger.strInput)
							{
								trigger.dAnimRatio = inputTrigger.dAnimRatio;
								isSame = true;
							}
						}
						// 같은게 없다면 데이터를 넣어준다.
						if (false == isSame)
						{
							m_InputTriggerList.push_back(inputTrigger);
							return true;
						}
					}
				}
			}			
		}
	}

	// 지금 바뀌지 않았다면 false를 리턴한다.
	return false;
}
void CAnimator::Set_RootBone(const _bool& _bIsRootBone)
{
	m_pCurrentClip->Set_RootBone(_bIsRootBone);
}
void CAnimator::Set_RootBoneName(_string _strRootBoneName)
{
	m_pCurrentClip->Set_RootBoneName(_strRootBoneName);
}

void CAnimator::Set_TargetDistance(const _float& _fTargetDistance)
{
	if (nullptr == m_pCurrentClip)
		return;

	m_pCurrentClip->Set_TargetDistance(_fTargetDistance);
}
void CAnimator::Add_WeaponBone(CHierarchyNode* pBone)
{
	m_WeaponBoneList.push_back(pBone);

	Safe_AddRef(pBone);
}
void CAnimator::Remove_WeaponBone(_uint iWeaponBoneIndex)
{
	if (iWeaponBoneIndex > m_WeaponBoneList.size())
		return;

	// 데이터를 지운다.
	m_WeaponBoneList.erase(m_WeaponBoneList.begin() + iWeaponBoneIndex);
}
#ifdef _DEBUG
// 디버그용 프로토타입 생성 함수 
HRESULT CAnimator::NativeConstruct_Prototype()
{
	return S_OK;
}

#endif // _DEBUG

/// <summary>
/// 경로를 받아와서 프로토타입을 만들어준다.
/// </summary>
/// <param name="strPath">애니메이터 데이터 파일 경로</param>
/// <returns></returns>
HRESULT CAnimator::NativeConstruct_Prototype(_wstring strPath)
{
	return S_OK;
}

HRESULT CAnimator::NativeConstruct_Clone(void* _pArg)
{
	if (nullptr != _pArg)
	{
		ANIMATORDESC animDesc = *static_cast<ANIMATORDESC*>(_pArg);

		_ulong dwByte = 0;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		PATHS paths = { L"Data", L"Animator", animDesc.strFilePath};
		FILEGROUP* AnimatorGroup = pGameInstance->Find_Folder(&paths);
		if (nullptr == AnimatorGroup)
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		// 모델을 장착한다.
		Set_Model(animDesc.pModel);
		// 애니메이터 컴포넌트를 들고있는 객체의 트랜스폼을 넣어준다.
		if (nullptr != animDesc.pTransform)
			Set_Transform(animDesc.pTransform);

		_wstring strPath = AnimatorGroup->FindData(animDesc.strFilePath)->strFilePath;

		// 경로를 받아온다.

		RELEASE_INSTANCE(CGameInstance);

		HANDLE hFile = CMethod::Begin_Load(strPath.c_str());

		_uint iClipSize = 0;
		ReadFile(hFile, &iClipSize, sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < (_uint)iClipSize; ++i)
		{
			CAnimationClip* pClip = CAnimationClip::Create();
			if (nullptr == pClip)
				return E_FAIL;

			// 애니메이션 클립의 이름을 받아온다.
			pClip->Set_Name(CMethod::Load_File_String(hFile, dwByte));
		
			// 메인 애니메이션을 받아서 넣습니다.
			_string strMainAnim = CMethod::Load_File_String(hFile, dwByte);
			_int iStart = (_int)strMainAnim.find("(", 0) + 1;
			_int iEnd = (_int)strMainAnim.find(")", 0) - iStart;
			strMainAnim = strMainAnim.substr(iStart, iEnd);
			pClip->Set_MainAnimation(animDesc.pModel->Get_Animation(strMainAnim));
			// 서브 애니메이션을 받아서 넣습니다.
			_string strSubAnim = CMethod::Load_File_String(hFile, dwByte);
			pClip->Set_SubAnimation(animDesc.pModel->Get_Animation(strSubAnim));

			// 루프 여부를 설정한다.
			_bool isLoop = false;
			ReadFile(hFile, &isLoop, sizeof(_bool), &dwByte, nullptr);
			pClip->Set_Loop(isLoop);

			// 루트본 여부를 설정한다.
			_bool isRootBone = false;
			ReadFile(hFile, &isRootBone, sizeof(_bool), &dwByte, nullptr);
			pClip->Set_RootBone(isRootBone);

			// 루트본 이름을 설정한다.
			pClip->Set_RootBoneName(CMethod::Load_File_String(hFile, dwByte));

			// 스피드값을 불러온다.
			_float fAnimSpeed = 0.f;
			ReadFile(hFile, &fAnimSpeed, sizeof(_float), &dwByte, nullptr);
			pClip->Set_AnimationSpeed(fAnimSpeed);

			// 애니메이션 타입값을 가져온다.
			pClip->Set_AnimationType(CMethod::Load_File_String(hFile, dwByte));

			// 이벤트 개수를 저장한다.
			_uint iEventSize = 0;
			ReadFile(hFile, &iEventSize, sizeof(_uint), &dwByte, nullptr);

			vector<CAnimationClip::ANIMEVENTDESC> eventList;

			// 이벤트를 저장한다.
			for (_uint k = 0; k < iEventSize; ++k)
			{
				CAnimationClip::ANIMEVENTDESC event;

				// 이벤트 타입을 불러온다.
				ReadFile(hFile, &event.tEventType, sizeof(_uint), &dwByte, nullptr);
				// 이벤트 이름을 불러온다
				event.strName = CMethod::Load_File_String(hFile, dwByte);
				// 이벤트가 시작되는 시간비율
				ReadFile(hFile, &event.dStartRatio, sizeof(_double), &dwByte, nullptr);
				// 이벤트가 끝나는 시간비율
				ReadFile(hFile, &event.dEndRatio, sizeof(_double), &dwByte, nullptr);
				// 이벤트를 끝낼 시간 ( 시작 후에 시간초로 끝내고 싶을때 )
				ReadFile(hFile, &event.dEndTime, sizeof(_double), &dwByte, nullptr);
				// Value값 저장
				ReadFile(hFile, &event.fValue, sizeof(_float), &dwByte, nullptr);
				// AddValue값 저장
				ReadFile(hFile, &event.fAddValue, sizeof(_float), &dwByte, nullptr);
				// 생성 위치
				ReadFile(hFile, &event.vEventPos, sizeof(_float4), &dwByte, nullptr);
				// 이벤트의 Dir
				ReadFile(hFile, &event.vEventDir, sizeof(_float4), &dwByte, nullptr);
				// 이벤트가가진 Values
				ReadFile(hFile, &event.vEventValues, sizeof(_float4), &dwByte, nullptr);
				// 사용할 뼈의 이름
				event.strEventText = CMethod::Load_File_String(hFile, dwByte);

				eventList.push_back(event);
			}
			// 생성된 클립에게 이벤트리스트를 넣어준다.
			pClip->Set_Event(eventList);
			// 생성된 클립에게 현재 모델 객체의 노드 리스트를 넣어준다.
			pClip->Set_HierarchyNodeList(m_pModelCom->Get_HierachyNodeList());
			this->Add_AnimationClip(pClip);
		}

		// 무기뼈 사이즈를 저장한다.
		_uint iWeaponBoneSize = 0;
		ReadFile(hFile, &iWeaponBoneSize, sizeof(_uint), &dwByte, nullptr);

		for (_uint j = 0; j < iWeaponBoneSize; ++j)
		{
			// 무기뼈에 사용할 뼈를 담아준다.
			CHierarchyNode* pBone = animDesc.pModel->Get_HierarchyNodes(CMethod::Load_File_String(hFile, dwByte).c_str());
			m_WeaponBoneList.push_back(pBone);
			//Safe_AddRef(pBone);
		}

		while (true)
		{
			TRANSITION transition;

			// 트랜지션의 EnterState
			ReadFile(hFile, &transition.tEnterState, sizeof(_uint), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			// EnterClip을 트랜지션에 넣어준다.
			transition.pEnterClip = this->Get_AnimationClip(CMethod::Load_File_String(hFile, dwByte));
			// ExitClip을 트랜지션에 넣어준다.
			transition.pExitClip = this->Get_AnimationClip(CMethod::Load_File_String(hFile, dwByte));
			
			// 트랜지션의 EnterRatio
			ReadFile(hFile, &transition.dEnterRatio, sizeof(_double), &dwByte, nullptr);
			// 트랜지션의 ExitRatio
			ReadFile(hFile, &transition.dExitRatio, sizeof(_double), &dwByte, nullptr);
			// 트랜지션의 ChangeRatio
			ReadFile(hFile, &transition.dChangeRatio, sizeof(_double), &dwByte, nullptr);
			// 트랜지션의 OverChange 불값
			ReadFile(hFile, &transition.bIsOverChangeRatio, sizeof(_bool), &dwByte, nullptr);
			// 트랜지션의 LerpSpeed
			ReadFile(hFile, &transition.dLerpSpeed, sizeof(_double), &dwByte, nullptr);
			// 트랜지션의 Trigger		
			transition.InputTrigger.strInput = CMethod::Load_File_String(hFile, dwByte);
			// 트랜지션의 Trigger Input Ratio
			ReadFile(hFile, &transition.InputTrigger.dAnimRatio, sizeof(_double), &dwByte, nullptr);
			// 트랜지션의 InputRatio 이상, 이하 판단
			ReadFile(hFile, &transition.InputTrigger.bIsOverRatio, sizeof(_bool), &dwByte, nullptr);

			this->m_TransitionList.push_back(transition);
		}		
		CloseHandle(hFile);
	}

	for (auto& transition : m_TransitionList)
	{
		// 해당 트랜지션이 ENTRY 라면
		if (ENTRY == transition.tEnterState)
		{
			if (nullptr != transition.pExitClip)
			{
				m_pCurrentClip = transition.pExitClip;
				m_bIsPlay = true;
				m_bIsTransitionUpdate = true;
				break;
			}
		}
	}

	return S_OK;
}

void CAnimator::Tick(const _double& _dTimeDelta)
{
	// 현재 클립이 nullptr이라면 return 한다.
	if (nullptr == m_pCurrentClip)
		return;

	// 메인 애니메이션이 비어있다면 return 한다.
	if (nullptr == m_pCurrentClip->Get_MainAnimation())
		return;
	
	// 플레이 중이라면 애니메이션의 업데이트를 진행한다.
	if (true == m_bIsPlay)
	{
		// 애니메이션 클립의 업데이트를 진행한다. ( 뼈등의 정보 업데이트 )
		Update_AnimationClip(_dTimeDelta);		

		// RootBone을 사용한다면 현재 위치를 담아준다.
		if (Is_RootBone())
		{
			if (false == m_pCurrentClip->Is_SupplementSituation())
			{
				_matrix OffSetMatrix = XMMatrixIdentity();
				_matrix RootBoneMatrix = XMMatrixIdentity();

				vector<CHierarchyNode*>vecHierachyNodes = m_pModelCom->Get_HierachyNodeList();
				/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
				for (auto& pHierarchyNode : vecHierachyNodes)
				{
					if ("Root" == pHierarchyNode->Get_Name())
					{
						OffSetMatrix = pHierarchyNode->Get_OffsetMatrix();
						RootBoneMatrix = pHierarchyNode->Get_RootTransformationMatrix();
						break;
					}
				}			
				OUTMATRIX matRootBone = CMethod::Out_Matrix(RootBoneMatrix);
				m_vRootBoneRotation = matRootBone.vRot;
				m_pCurrentClip->Set_RootBoneLook(XMLoadFloat3(&m_vRootBoneRotation));

				 // Owner 객체의 월드행렬을 받아온다.
				 _matrix WorldMat = m_pOwnerTransformCom->Get_WorldMatrix();
				 _vector vBeginPos = CMethod::XMMatrix4x4GetVector(m_vBeginTransformMatrix, MATROW_POS);
				 WorldMat = CMethod::XMMatrixSetVector(vBeginPos, MATROW_POS, WorldMat);
				 // 소켓 매트릭스로 만든다.
				 MATRIXES matrixes = { OffSetMatrix, RootBoneMatrix, m_pModelCom->Get_PivotMatrix() , WorldMat };

				 _matrix BoneMatrix = CMethod::XMMatrixMultiple_P(matrixes);

				 m_vRootBoneMatrix = CMethod::XMMatrixStoreFloat4x4_P(BoneMatrix);   
			}
		}
		// BoneTransform을 사용하는 경우에 프레임당 이동값을 계산해준다.
		if (Is_BoneTransform())
		{
			if (nullptr != m_pOwnerTransformCom)
			{
				if (nullptr == m_pCurrentClip->Get_OwnerTransform())
					return m_pCurrentClip->Set_OwnerTransform(m_pOwnerTransformCom);

				// 현재 재생중인 클립에 RootTransform을 계산하기위한 Dir값을 보낸다.
				m_pCurrentClip->Set_RootTransformDir(m_pOwnerTransformCom->Get_Look());
				// 0이 아닐때만 움직인다.
				if (!CMethod::Is_Vector_Zero(m_pCurrentClip->Get_RootTransform()))
				{
					// Owner 객체의 월드행렬을 받아온다.
					_float4x4 WorldMat = m_pOwnerTransformCom->Get_WorldFloat4x4();
					// 구해온 World행렬에 위치값에 0을 곱하여 위치값을 0으로만든다.
					WorldMat = CMethod::XMMatrix4x4SetFloat3(_float3(0.f, 0.f, 0.f), MATROW_POS, WorldMat);
					// 위치값이 0으로된 World행렬을 이용하여 프레임당 이동값을 구한다.
					MATRIXES matrixes = { CMethod::XMMatrixLoadFloat4x4_P(WorldMat),  m_pModelCom->Get_PivotMatrix(), m_pCurrentClip->Get_RootBoneMatrix() };
					m_vBoneTransformPos = m_pCurrentClip->Get_RootTransform() + CMethod::XMMatrix4x4GetVector(CMethod::XMMatrixMultiple4x4_P(matrixes), MATROW_POS);
				}
				else
					m_vBoneTransformPos = _float4(0.f, 0.f, 0.f, 0.f);
			}
		}

		// 사이즈가 0이라면 한번 채워준다. 
		if (0 == m_ActiveTransitionList.size() || true == m_bIsTransitionUpdate)
		{
			// 미리 ActiveTransition들을 담아준다.
			m_ActiveTransitionList.clear();
			for (auto& Transition : m_TransitionList)
			{
				// 현재 돌고있는 클립이 Enter와 같은경우 ( 자기 자신이라면 )
				if (Transition.pEnterClip == m_pCurrentClip)
				{
					m_ActiveTransitionList.push_back(Transition);
				}
				// 아무 상태에서나 들어갈 수 있는 트랜지션들은 Trigger가 들어올 때 체크하자
				else if (ANYSTATE == Transition.tEnterState)
				{
					m_ActiveTransitionList.push_back(Transition);
				}
				else if (ENTRY == Transition.tEnterState)
					m_ActiveTransitionList.push_back(Transition);

				// 어느 상태건 Entry로 돌아갈 수 있다는것을 잊지말자.
			}
			m_bIsTransitionUpdate = false;
		}

		// Owner 객체가 현재 이동하는 이동방향벡터를 전달해준다.
		m_pCurrentClip->Set_OwnerDirection(XMLoadFloat3(&m_vOwnerDirection));
		// 애니메이션 클립의 정보를 Update한다.
		m_pCurrentClip->Tick(_dTimeDelta);
		
		// 트랜지션의 발동 조건 조사를 한다.
		Check_Transition_Condition();

		// 애니메이션 클립이 끝났을 경우 애니메이션 클립을 바꾼다.
		if (m_pCurrentClip->Is_Finish())		
			Change_AnimationClip();	
	}
}

void CAnimator::Late_Tick(const _double& _dTimeDelta)
{
}

void CAnimator::Change_AnimationClip()
{	
	// 현재 애니메이션이 없다면 return한다.
	if (nullptr == m_pCurrentClip)
		return;

	if (true == m_pCurrentClip->Is_RootBone())
	{
		// RootBone 애니메이션이었다면 부모객체의 Look을 m_vRootBoneMatrix의 Look으로 변경한다. ( 테스트 코드입니다. )				
		if (nullptr != m_pOwnerTransformCom)
		{
			m_pOwnerTransformCom->Rotate_TurnToNotFix(m_vRootBoneRotation);
			m_vRootBoneRotation = _float3(0.f, 0.f, 0.f);
		}
	}

	// 다음 넘어갈 다이렉트 클립이 있다면 Set으로 다음 애니메이션으로 넘어간다. ( 보간을 하지 않는다 )
	if (nullptr != m_pNextDirectClip)
	{
		Set_CurrentAnimationClip(m_pNextDirectClip);
		m_pNextDirectClip = nullptr;
	}

	CAnimationClip* pFindClip = nullptr;

	// 길에 끝에 도달했을때 자기자신을 Enter로 하는 Clip이 없다면 Entry로 되돌린다.
	for (auto& Transition : m_ActiveTransitionList)
	{
		if (nullptr == Transition.pEnterClip)
			continue;

		// 현재 재생되고 있는 클립과 같은 EnterClip을 가진 트랜지션이 없다면 nullptr 반환, 있다면 담아줌.
		if (m_pCurrentClip == Transition.pEnterClip)
		{
			pFindClip = Transition.pEnterClip;
			break;
		}
	}

	//FindClip이 nullptr이라면 Loop 여부를 체크한 후 Loop가 아니라면 해당 Play를 false로 하고 Loop 상태라면 해당 클립을 계속하여 돌린다.
	if (nullptr == pFindClip)
	{
		// Loop상태라면 계속하여 돌린다.
		if (m_pCurrentClip->Is_Loop())		
			return;		
		// Loop가 아니라면 Play를 False로 바꾼다.
		else
		{				
			m_bIsPlay = false;
			// 처음 부분으로 되돌려야하니 Bone상태를 업데이트한다.
			Update_InitBoneState();
		}
	}

	TRANSITION EntryTransition;

	// 현재 자기 자신으로부터 빠져나갈 Transition이 존재할 경우 해당 Transition을 이용해서 애니메이션을 재생시킨다.
	for (auto& Transition : m_ActiveTransitionList)
	{
		// 상태가 Entry인 트랜지션은 패스한다.
		if (ENTRY == Transition.tEnterState)
		{
			EntryTransition = Transition;
			continue;
		}

		// EnterClip과 현재 CurrentClip이 같은데 CurrentClip이 트리거를 갖고있지 않다면 NextClip으로 바로 바꾼다.
		if ("" == Transition.InputTrigger.strInput)
		{
			// 다음 애니메이션 클립을 설정한다.
			m_pNextClip = Transition.pExitClip;
			m_pCurTransition = &Transition;
			// Finish 초기화
			m_pCurrentClip->Set_Finish(false);
			m_pCurrentClip->Get_MainAnimation()->Set_SupplementSituation(false);
			if(nullptr != m_pNextClip)
				m_pNextClip->Set_Finish(false);
			return;
		}
		// 트리거가 존재하는 경우
		else
		{
			// 현재 재생중인 클립과 EnterClip이 같은 경우의 트랜지션들만 비교한다.
			for (auto& inputTrigger : m_InputTriggerList)
			{
				// 인풋 트리거중 Input값이 같은것이 있다면 ratio값을 비교하여 true가 들어오면 Transition을 가져온다.
				if (true == Transition.Compare_Trigger(inputTrigger))
				{
					// 다음 트랜지션을 찾으면 Clip을 변경한다.
					// 다음 애니메이션 클립을 설정한다.
					m_pNextClip = Transition.pExitClip;
					m_pCurTransition = &Transition;
					// Finish 초기화
					m_pCurrentClip->Set_Finish(false);
					m_pCurrentClip->Get_MainAnimation()->Set_SupplementSituation(false);
					m_pNextClip->Set_Finish(false);
					return;
				}
			}
		}
	}
	// Loop상태라면 계속하여 돌린다.
	if (m_pCurrentClip->Is_Loop())
	{
		m_pCurrentClip->Initialize_Data();
		return;
	}

	// 모든 조건을 통과하지 못했다면 Entry로 되돌린다. ( 트리거가 있으나 트리거의 입력 실패가 보통 이유가 됩니다. )
	m_pNextClip = EntryTransition.pExitClip;
	return;
}

void CAnimator::Update_AnimationClip(const _double& _dTimeDelta)
{
	// 등록된 애니메이션 클립이 없다면 return한다.
	if (0 == m_AnimationClips.size())
		return;

	// 등록된 클립이 없다면 클립 리스트의 0번 클립으로 설정해준다.
	if (nullptr == m_pCurrentClip)			
		m_pCurrentClip = m_AnimationClips[0];
	

	// 다음 진행할 클립이 있다면 다음 애니메이션 클립으로 설정해준다.
	if (nullptr != m_pNextClip)
	{
		// 다음 애니메이션 클립과 비교하여 업데이트 해준다. ( CurTransition이 없다면 그냥 돌리고 있다면 NextClip Ratio값을 보낸다.
		if(nullptr != m_pCurTransition)
			m_pCurrentClip->Update_NextAnimation(_dTimeDelta, m_pCurTransition->dExitRatio,m_pCurTransition->dLerpSpeed, m_pNextClip);
		else
			m_pCurrentClip->Update_NextAnimation(_dTimeDelta, m_pNextClip);
		if (false == m_pCurrentClip->Is_SupplementSituation())
		{
			// 선형보간이 끝났다면 현재 클립의 데이터들을 초기화해준다.
			m_pCurrentClip->Initialize_Data();		
			// 현재 클립은 PrevClip으로 바꾼다.
			m_pPrevClip = m_pCurrentClip;
			// 만약 현재 재생중이었던 클립이 RootBone 애니메이션이라면 AdjustPos값을 지운다.
			if (true == m_pCurrentClip->Is_RootBone())
			{
				m_vAdjustPos = _float4(0.f, 0.f, 0.f, 0.f);
			}
			// 애니메이션 이벤트로 만들어진 이동값이 존재한다면 없앤다.
			m_vBoneTransformPos = _float4(0.f, 0.f, 0.f, 0.f);
			// 선형보간이 끝났다면 현재 클립을 다음 클립으로 변경한다.
			m_pCurrentClip = m_pNextClip;
			// 다음 클립은 nullptr로 채워준다.
			m_pNextClip = nullptr;
			// 현재 트랜지션을 nullptr로 채워준다.
			m_pCurTransition = nullptr;
			// 다음 클립으로 바뀌으니 여태까지 들어온 InputTrigger들을 비워준다.
			m_InputTriggerList.clear();
			// ActiveTransition들을 Clear한다.
			m_ActiveTransitionList.clear();
			// 미리 ActiveTransition들을 담아준다.
			for (auto& Transition : m_TransitionList)
			{
				// 현재 돌고있는 클립이 Enter와 같은경우 ( 자기 자신이라면 )
				if (Transition.pEnterClip == m_pCurrentClip)				
					m_ActiveTransitionList.push_back(Transition);				
				// 아무 상태에서나 들어갈 수 있는 트랜지션들은 Trigger가 들어올 때 체크하자
				else if (ANYSTATE == Transition.tEnterState)				
					m_ActiveTransitionList.push_back(Transition);				
				else if (ENTRY == Transition.tEnterState)
					m_ActiveTransitionList.push_back(Transition);
			}
			// 트랜스폼 컴포넌트를 갖고있다면 주인 객체의 현재 위치를 받아온다.
			if (nullptr != m_pOwnerTransformCom)
			{
				m_vBeginTransformMatrix = m_pOwnerTransformCom->Get_WorldFloat4x4();
				// 현재 클립이 OwnerTransform이 없다면 채워준다.
				if (nullptr == m_pCurrentClip->Get_OwnerTransform())
					return m_pCurrentClip->Set_OwnerTransform(m_pOwnerTransformCom);
			}

			// 애니메이션 Change를 true로 해준다. ( 다른쪽에서 사용가능할 수 있어서 만든 bool값 )
			m_bIsChange = true;
			// 애니메이션 Transition목록을 초기화 하기 위해 bool변수를 true로 만든다.
			m_bIsTransitionUpdate = true;
			/* 현재 애님을 재생하기위한 채널(뼈)들을 시간에 맞는 상태로 뼈들의 TransformationMatrix 갱신한다.  */
			m_pCurrentClip->Update_TransformMatrices();
		}
	}
	else
	{
		/* 현재 애님을 재생하기위한 채널(뼈)들을 시간에 맞는 상태로 뼈들의 TransformationMatrix 갱신한다.  */
		m_pCurrentClip->Update_TransformMatrices(_dTimeDelta);

		// 애니메이션 Change를 다시 false로 바꿔준다. ( 다른쪽에서 사용가능할 수 있어서 만든 bool값 )
		m_bIsChange = false;
	}

	// 모델 객체의 뼈값을 업데이트한다.
	if (nullptr != m_pModelCom)
	{
		vector<CHierarchyNode*>vecHierachyNodes = m_pModelCom->Get_HierachyNodeList();
		/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
		for (auto& pHierarchyNode : vecHierachyNodes)
		{	
			pHierarchyNode->Update_CombinedTransformationMatrix();
		}
	}
}

void CAnimator::Check_Transition_Condition()
{
	// 자신이 갖고있는 트리거 string값과 같은 Input트리거가 들어와있는 트랜지션 목록들의 조건 검사를 한다.
	for (auto& Transition : m_ActiveTransitionList)
	{
		// 상태가 Anystate가 아닌 ActiveTransition 목록중 검사한다.
		if (ANYSTATE != Transition.tEnterState)
		{
			// Entry도 따로 돌지 않는다.
			if (ENTRY != Transition.tEnterState)
			{
				// 호출 트리거가 없다면
				if ("" == Transition.InputTrigger.strInput)
				{
					// EnterClip의 진행도가 트랜지션이 갖고있는 EnterRatio보다 크다면 다음 클립을 Exit로 넣어준다.
					if (Transition.dEnterRatio <= Transition.pEnterClip->Get_Ratio())
					{
						// 다음 애니메이션 클립을 Transition의 ExitClip으로 설정한다.
						Set_NextAnimClip(Transition.pExitClip);
						// 현재 트랜지션또한 변경해준다.
						m_pCurTransition = &Transition;
						return;
					}
				}
			}
			
			for (auto& input : m_InputTriggerList)
			{
				// 호출 트리거가 같고 input time이 조건에 만족한다면
				if (true == input.bIsOverRatio)
				{
					if (input.strInput == Transition.InputTrigger.strInput && input.dAnimRatio <= Transition.InputTrigger.dAnimRatio) //&& input.dAnimRatio >= Transition.InputTrigger.dAnimRatio)
					{
						// ChangeRatio 가 넘어가야 조건이 충족되는 경우
						if (true == Transition.bIsOverChangeRatio)
						{
							// EnterClip의 진행도가 트랜지션이 갖고있는 EnterRatio보다 크다면 다음 클립을 Exit로 넣어준다.
							if (Transition.dChangeRatio <= Transition.pEnterClip->Get_Ratio())
							{
								// 다음 애니메이션 클립을 Transition의 ExitClip으로 설정한다.
								Set_NextAnimClip(Transition.pExitClip);
								// 현재 트랜지션또한 변경해준다.
								m_pCurTransition = &Transition;
								return;
							}
						}
						else
						{
							// EnterClip의 진행도가 트랜지션이 갖고있는 EnterRatio보다 크다면 다음 클립을 Exit로 넣어준다.
							if (Transition.dChangeRatio >= Transition.pEnterClip->Get_Ratio())
							{
								// 다음 애니메이션 클립을 Transition의 ExitClip으로 설정한다.
								Set_NextAnimClip(Transition.pExitClip);
								// 현재 트랜지션또한 변경해준다.
								m_pCurTransition = &Transition;
								return;
							}
						}
					}
				}
				else
				{
					if (input.strInput == Transition.InputTrigger.strInput && input.dAnimRatio >= Transition.InputTrigger.dAnimRatio) //&& input.dAnimRatio >= Transition.InputTrigger.dAnimRatio)
					{
						// ChangeRatio 가 넘어가야 조건이 충족되는 경우
						if (true == Transition.bIsOverChangeRatio)
						{
							// EnterClip의 진행도가 트랜지션이 갖고있는 EnterRatio보다 크다면 다음 클립을 Exit로 넣어준다.
							if (Transition.dChangeRatio <= Transition.pEnterClip->Get_Ratio())
							{
								// 다음 애니메이션 클립을 Transition의 ExitClip으로 설정한다.
								Set_NextAnimClip(Transition.pExitClip);
								// 현재 트랜ksmdgkrp지션또한 변경해준다.
								m_pCurTransition = &Transition;
								return;
							}
						}
						else
						{
							// EnterClip의 진행도가 트랜지션이 갖고있는 EnterRatio보다 크다면 다음 클립을 Exit로 넣어준다.
							if (Transition.dChangeRatio >= Transition.pEnterClip->Get_Ratio())
							{
								// 다음 애니메이션 클립을 Transition의 ExitClip으로 설정한다.
								Set_NextAnimClip(Transition.pExitClip);
								// 현재 트랜지션또한 변경해준다.
								m_pCurTransition = &Transition;
								return;
							}
						}
					}
				}
			}
		}
	}
}

void CAnimator::Update_InitBoneState()
{
	if (nullptr == m_pCurrentClip)
		return;

	// 트랜스폼 컴포넌트를 갖고있다면 주인 객체의 현재 위치를 받아온다.
	if (nullptr != m_pOwnerTransformCom)
		m_vBeginTransformMatrix = m_pOwnerTransformCom->Get_WorldFloat4x4();

	// 0.0으로 애니메이션 업데이트를 돌려서 뼈값만 설정해준다.
	m_pCurrentClip->Update_TransformMatrices(0.0);

	// 모델 객체의 뼈값을 업데이트한다.
	if (nullptr != m_pModelCom)
	{
		vector<CHierarchyNode*>vecHierachyNodes = m_pModelCom->Get_HierachyNodeList();
		/* 부모로부터 자식뼈에게 누적시켜 전달한다.(CombinedTransformationMatrix) */
		for (auto& pHierarchyNode : vecHierachyNodes)
			pHierarchyNode->Update_CombinedTransformationMatrix();
	}
}

CAnimator* CAnimator::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _wstring strPath)
{
	CAnimator* pInstance = new CAnimator(_pDevice, _pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(strPath)))
	{
		MSG_BOX("Failed to Created : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimator* CAnimator::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CAnimator* pInstance = new CAnimator(_pDevice, _pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(L"")))
	{
		MSG_BOX("Failed to Created : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CAnimator::Clone(void* pArg)
{
	CAnimator* pInstance = new CAnimator(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
	//	MSG_BOX("Failed to Cloned : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimator::Free()
{
	// 모델을 Safe_Release한다.
	Safe_Release(m_pModelCom);

	// 애니메이션 클립을 Release한다.
	for (auto& pAnimClip : m_AnimationClips)
		Safe_Release(pAnimClip);

	m_AnimationClips.clear();

	__super::Free();
}

_bool CAnimator::tagTransition::Compare_Trigger(INPUTTRIGGER _trigger)
{
	// Trigger의 비교값이 모두 같다면 true를 반환한다.

	// InputRatio보다 현재 애니메이션의 Ratio가 이상인 경우에 Trigger를 받아온다.
	if (true == this->InputTrigger.bIsOverRatio)
	{
		if (this->InputTrigger.strInput == _trigger.strInput && this->InputTrigger.dAnimRatio <= _trigger.dAnimRatio)
			return true;
	}
	// InputRatio보다 현재 애니메이션의 Ratio가 이하인 경우에 Trigger를 받아온다.
	else
	{
		if (this->InputTrigger.strInput == _trigger.strInput && this->InputTrigger.dAnimRatio >= _trigger.dAnimRatio)
			return true;
	}
	return false;	
}

/// <summary>
/// 그룹에 등록되어있는 애니메이션 클립을 반환합니다.
/// </summary>
/// <param name="fRatio">0 ~ 1 사이의 값으로 된 비율 값</param>
/// <returns></returns>
CAnimationClip* CAnimator::tagCutAnimationGroup::Get_AnimationClip(_float fRatio)
{
	// 지금 Ratio기준으로의 TimeAcc를 구합니다.
	_float fTimeAcc = fRatio * (_float)this->Get_Duration();

	// 애니메이션 클립리스트를 받아온다.
	if (0 == this->AnimationClipList.size())
		return nullptr;

	// Clip들의 Duration값을 담을 변수
	_double dDuration = 0.0;
	
	for (_uint i = 0; i < (_uint)this->AnimationClipList.size(); ++i)
	{
		dDuration += this->AnimationClipList[i]->Get_Duration();

		// 더해진 duration값과 인자로들어옴 TimeAcc값의 차가 0이 넘을경우 해당 클립을 리턴한다.
		if (0 > dDuration - fTimeAcc)
			return this->AnimationClipList[i];
	}

	return nullptr;
}

/// <summary>
/// 현재 그룹이 갖고있는 트랜지션들의 애니메이션 클립들이 갖고있는 Duration의 총 합
/// </summary>
/// <returns></returns>
_double CAnimator::tagCutAnimationGroup::Get_Duration()
{
	if (0 == AnimationClipList.size())
		return 0.f;

	_double dAllDuration = 0.0;

	// 애니메이션 리스트로 애니메이션 클립들의 총 플레이 타임을 구합니다.
	for (auto& pClip : AnimationClipList)
	{
		_double dDuration = pClip->Get_Duration();

		dAllDuration += dDuration;
	}

	return dAllDuration;
}

/// <summary>
/// 현재 TimeAcc를 이용하여 비율을 가져옵니다.
/// </summary>
/// <returns></returns>
_float CAnimator::tagCutAnimationGroup::Get_CurrentRatio()
{
	return _float(this->dGroupTimeAcc / this->Get_Duration());
}

/// <summary>
/// 총 플레이 타임을 구합니다. ( TimeAcc 값 )
/// </summary>
/// <returns></returns>
_float CAnimator::tagCutAnimationGroup::Get_PlayTime()
{
	if (0 == AnimationClipList.size())
		return 0.f;

	_double dPlayTime = 0.0;

	// 애니메이션 리스트로 애니메이션 클립들의 총 플레이 타임을 구합니다.
	for (auto& pClip : AnimationClipList)
	{
		_double dTickPerSecond =  pClip->Get_TickPerSecond();
		_double dDuration = pClip->Get_Duration();

		dPlayTime += dDuration / dTickPerSecond;
	}

	return (_float)dPlayTime;
}
