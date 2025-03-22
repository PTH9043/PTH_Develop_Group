#pragma once
#include "Component.h"

BEGIN(Engine)
class CAnimationClip;
class CModel;
class CHierarchyNode;

class ENGINE_DLL CAnimator final : public CComponent
{
public:
	typedef struct tagAnimatorDesc
	{
		CModel* pModel = nullptr;
		CTransform* pTransform = nullptr;
		_wstring strFilePath = L"";
	}ANIMATORDESC;

	// 들어오는 트리거 인풋 구조체
	typedef struct tagInputTrigger
	{
		// 애니메이션 비율 ( 애니메이션의 비율이 언제일때 이 트리거가 들어왔는지? )
		_double dAnimRatio = 0.0;
		// 인풋 트리거
		_string	strInput = "";
		// 애니메이션의 진행도가 AnimRatio의 이상일때 들어왔는지 이하일때 들어왔는지 판단하기위한 붋값 => true면 이상 , false면 이하
		_bool	bIsOverRatio = true;
	}INPUTTRIGGER;

	// 진입점 클립의 상태
	enum ENTERSTATE
	{
		ENTRY,		// 시작 ( IDLE의 경우 이것을 사용 )
		ANYSTATE,	// 아무 상태에서건 다 가능 ( HIT의 경우에 사용 )
		DEFAULT,	// 진입점 클립이 다른 클립인 경우 ( 위의 2가지 경우는 진입점 클립이 없음. )
		ENTERSTATE_END
	};

	// 애니메이션 클립간의 진입, 진출 관계도를 만들 수 있는 구조체.
	typedef struct tagTransition
	{
		// Enter상태가 어떤것인지 구분하기위한 EnumState
		ENTERSTATE		tEnterState = ENTERSTATE_END;
		// 진입점 클립이 존재할 경우 Animator의 클립이 진입점 클립인 경우에만 다음 클립으로 현재 클립이 들어올 수 있음.
		// 진입점 클립이 존재하지 않는 경우에는 어떤 상태에서도 Trigger를 통해 들어올 수 있음.
		CAnimationClip* pEnterClip = nullptr;
		/*진출 클립이 존재할 경우 애니메이션이 종료 된다면 객체의 다음 애니메이션 클립을 진출 클립으로 되돌립니다.
		만약 진입클립과 진출클립이 모두 존재하지 않는 경우에는 별다른 트리거가 발생하지 않는 이상은 해당 클립을
		반복하여 재생하게 됩니다.*/
		CAnimationClip* pExitClip = nullptr;
		// EnterClip의 보간을 시작할 애니메이션 진행도 ( 0 ~ 1 )
		_double			dEnterRatio = 1.0;
		// ExitClip의 보간을 시작할 애니메이션 진행도 ( 0 ~ 1 )
		_double			dExitRatio = 0.0;
		// Trigger가 들어왔을 경우 애니메이션이 교체될 비율값 ( 기본을 0으로 하여 설정해주지 않으면 바로 교체되도록, 끝나고 발동하는거면 1.0으로 )
		_double			dChangeRatio = 0.0;
		// ChangeRatio로 비교할 경우 True면 ChangedRatio보다 클때, False면 작을때로 설정한다. 기본은 True로한다.
		_bool			bIsOverChangeRatio = true;
		// 보간 속도 ( 기본이 1.0임 )
		_double			dLerpSpeed = 1.0;
		// Transition을 판단할 인풋 트리거
		INPUTTRIGGER	InputTrigger;

		/// <summary>
		/// 트리거를 비교한다.
		/// </summary>
		/// <param name="_trigger">비교할 트랜지션의 트리거</param>
		/// <returns></returns>
		_bool Compare_Trigger(INPUTTRIGGER _trigger);
	}TRANSITION;

	// 여러가지 상황별 트랜지션들을 하나로 묶어서 보기위한 그룹 ( 주로 컷씬등의 상황에서 사용 )
	typedef struct tagCutAnimationGroup
	{
		// 그룹의 이름
		_string					strGroupName = "";
		// 등록된 트랜지션 목록
		vector<CAnimationClip*>	AnimationClipList;
		// 등록된 트랜지션들의 총 플레이타임
		_float					fPlayTime = 0.f;
		// 그룹을 돌리는 TimeAcc값 
		_double					dGroupTimeAcc = 0.0;


		// 원하는 시간대의 애니메이션 클립을 가져옵니다.
		CAnimationClip*			Get_AnimationClip(_float fRatio);
		// 등록된 애니메이션 클립들의 총 Duration을 계산해서 가져옵니다.
		_double					Get_Duration();
		// 등록된 애니메이션 클립들의 총 Duration을 0 ~ 1 사이의 값으로 조절한 비율을 받아옵니다.
		_float					Get_CurrentRatio();
		// 등록된 트랜지션들의 총 플레이 타임을 계산해서 가져옵니다.
		_float					Get_PlayTime();

	}CUTANIMGROUP;

private:
	explicit CAnimator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CAnimator(const CAnimator& _rhs);
	virtual ~CAnimator() = default;

public:
	// 현재 애니메이션 클립의 모델을 받아옵니다.
	CModel* Get_Model() { return m_pModelCom; }
	// 현재 애니메이터 컴포넌트가 갖고있는 애니메이션 클립 목록을 받아옵니다.
	vector<CAnimationClip*> Get_AnimationClipList() { return m_AnimationClips; }
	// 현재 재생되고있는 애니메이션 클립을 받아옵니다.
	CAnimationClip* Get_CurrentAnimationClip() { return m_pCurrentClip; }
	// 다음 재생될 애니메이션 클립을 받아옵니다.
	CAnimationClip* Get_NextAnimationClip();
	// 이전에 재생되고 있는 애니메이션 클립
	CAnimationClip* Get_PrevAnimationClip() { return m_pPrevClip; }
	// string 값으로 애니메이션 클립을 찾아옵니다.
	CAnimationClip* Get_AnimationClip(_string _strName);
	// Transition이 조건에 맞춰서 발동됐을때 저장되는 CurTransition의 Input값을 받아옵니다.
	_string Get_CurrentTrigger();
	// 현재 재생되고있는 애니메이션 클립의 MainAnimation 이름을 받아옵니다.
	_string Get_MainAnimationName();
	// 현재 재생되고있는 애니메이션 클립의 SubAnimation 이름을 받아옵니다.
	_string Get_SubAnimationName();
	// RootBoneMatrix를 생성하는 Bone의 이름을 받아옵니다.
	_string Get_RootBoneName() { return m_strRootBoneName; }
	// 현재 재생중인 Clip이 가진 AnimationType을 받아옵니다.
	const _string& Get_AnimationType();
	// 트랜지션 목록을 받아옵니다.
	vector<TRANSITION> Get_TransitionList() { return m_TransitionList; }
	// 액티브 트렌지션 리스트
	const vector<TRANSITION>& Get_ActiveTransitionList() { return m_ActiveTransitionList; }
	// WeaponBone 리스트를 받아옵니다.
	vector<CHierarchyNode*> Get_WeaponBoneList();
	// WeaponBone을 받아옵니다.
	CHierarchyNode* Get_WeaponBone();
	// 현재 플레이중인 애니메이션 클립의 진행도를 받아온다.
	_double Get_Ratio();
	// 현재 플레이중인 애니메이션 클립의 Duration값을 받아옵니다.
	_double Get_Duration();
	// 현재 플레이중인 애니메이션 클립의 TimePerSecond값을 받아옵니다.
	_double Get_TickPerSecond();
	// 현재 플레이중인 애니메이션 클립의 TimeAcc값을 받아옵니다.
	_double Get_TimeAcc();
	// 현재 플레이중인 애니메이션의 재생속도값을 받아옵니다.
	_float	Get_AnimationSpeed();
	// 현재 플레이중인 애니메이션의 RootBone 이동값을 받아옵니다.
	_matrix Get_RootBoneMatrix();
	// 현재 플레이중인 애니메이션의 BoneTransform 이벤트로 계산된 프레임당 이동값을 반환합니다.
	_vector Get_BoneTransformPos() { return m_vBoneTransformPos.Get_Vector(); }	
	// 현재 플레이중인 애니메이션의 BoneTransfrom 이벤트에서 등록되어있는 최대 이동거리를 받아옵니다.
	_float Get_MoveDistance();
	// Get Move Force
	_float Get_MoveForce();
	// 콜라이더가 가진 방향값을 반환합니다.
	_vector Get_ColliderDir();
	// 콜라이더가 가진 Scale값을 반환합니다.
	_vector Get_ColliderScale();
	// 콜라이더가 가진 위치값을 반환합니다.
	_vector Get_ColliderPos();
	// 애니메이션 룩 반환
	_float3 Get_RootBoneLook();

	// 현재 돌고있는 애니메이션 클립의 진행에 따른 현재 콜라이더 활성화 여부
	const _bool& Is_AttackColliderActive();
	// 현재 돌고있는 애니메이션 클립의 콜라이더 PushForce
	const _float&	Get_PushedForce();
	// 현재 돌고있는 애니메이션 클립의 콜라이더 Height
	const _float&	Get_Height();
	// 현재 돌고있는 애니메이션 클립의 특정 이벤트가 진행되는 총 시간 (실제시간)
	const _double&	Get_EventPlayTime();
	// 현재 돌고있는 애니메이션 클립의 특정 이벤트가 진행되는지를 체크하는 불값
	const _bool&	Is_EventPlay();
	// 현재 애니메이터의 Play 여부를 반환합니다.
	const _bool& Is_Play() { return m_bIsPlay; }
	// 현재 돌고있는 애니메이션 클립이 RootBone 애니메이션을 적용하는지에 대한 여부를 반환합니다.
	_bool Is_RootBone();
	// 현재 돌고있는 애니메이션 클립이 BoneTransForm 애니메이션을 적용하는지에 대한 여부를 반환합니다.
	_bool Is_BoneTransform();
	// 현재 돌고있는 애니메이션 클립 이벤트로 중력을 적용하고있는지를 체크합니다.
	_bool Is_Gravity();
	// 현재 돌고있는 애니메이션 클립 이벤트로 메쉬의 활성화 여부를 체크합니다.
	_bool Is_MeshActive();
	// 현재 돌고있는 애니메이션 클립 이벤트로 트레일의 활성화 여부를 체크합니다.
	_bool Is_TrailActive();
	// 트레일의 Start 와 End 값으로 사용할 값을 받아옵니다.
	_float2 Get_TrailValues();
	
	const _float3& Get_RealBonePosition() const { return m_vRealBonePosition; }
	// 애니메이터 컴포넌트가 BoneDeltaPos를 계산할 트랜스폼 컴포넌트를 설정해줍니다.
	void Set_Transform(CTransform* pTransform);
	// 현재 애니메이션 클립의 모델을 설정합니다.
	void Set_Model(CModel* pModel);
	// 현재 플레이중인 애니메이션 클립의 진행도를 변경합니다.
	void Set_Ratio(_double dRatio);
	// 애니메이션 클립을 Update 할지 말지 결정하는 bool값을 조정합니다.
	void Set_Play(const _bool& isPlay) { m_bIsPlay = isPlay; }
	// 애니메이션 클립을 멈춥니다.
	void Set_Stop();	
	// 애니메이션의 TimeAcc값을 설정해줍니다.
	void Set_TimeAcc(_double dTimeAcc);
	// 재생 될 애니메이션 클립을 설정합니다.
	void Set_CurrentAnimationClip(CAnimationClip* pClip);
	// 재생 될 애니메이션 클립을 설정합니다. ( 이름으로 설정 )
	void Set_CurrentAnimationClip(_string _strName);
	// 다음 재생 될 애니메이션 클립을 설정합니다. => 컷씬 등에서 사용
	void Set_NextDirectAnimationClip(CAnimationClip* pClip);
	// 다음 재생 될 애니메이션 클립을 설정합니다. ( 이름으로 설정 ) => 컷씬 등에서 사용
	void Set_NextDirectAnimationClip(const _string _strName);

	// 애니메이션 클립을 Entry로 되돌립니다.
	void Set_AnimationClipToEntry();
	// 애니메이션 클립을 추가합니다
	void Add_AnimationClip(CAnimationClip* _pClip);
	// 애니메이션 클립을 삭제합니다
	void Delete_AnimationClip(_string strName);
	// 클립 리스트를 세팅합니다.
	void Set_AnimationClipList(vector<CAnimationClip*> clipList) { m_AnimationClips = clipList; }
	// 트랜지션을 추가합니다.	
	void Make_Transition();
	// 트랜지션 목록을 설정합니다.
	void Set_TransitionList(vector<TRANSITION> transitionList) { m_TransitionList = transitionList; Update_TransitionList(); }
	// 트랜지션 목록을 최신화합니다.
	void Update_TransitionList() { m_bIsTransitionUpdate = true; }
	// 인풋 트리거를 받아옵니다. ( 내부에서 만약 들어온 트리거에 해당되는 Transition이 있다면 클립을 변경시킵니다. )
	_bool Input_Trigger(_string _trigger);	
	// RootBone애니메이션의 사용 여부를 설정합니다.
	void Set_RootBone(const _bool& _bIsRootBone);
	// RootBoneMatrix를 만들 RootBone의 이름을 설정합니다.
	void Set_RootBoneName(_string _strRootBoneName);
	// AdjustTransformPos를 조정한다.
	void Set_AdjustTransformPos(_vector _vPos) { XMStoreFloat4(&m_vAdjustPos,_vPos); }
	// 애니메이터를 가진 객체가 움직이는 방향값을 세팅합니다.
	void Set_OwnerDirection(_vector _vDir) { XMStoreFloat3(&m_vOwnerDirection, _vDir); }
	// 애니메이션의 이동값에 이용할 상대방과의 거리값을 설정합니다.
	void Set_TargetDistance(const _float& _fTargetDistance);
	// 무기에 쓸 뼈 정보를 리스트에 담는다.
	void Add_WeaponBone(CHierarchyNode* pBone);	
	// WeaponBoneList에서 지우고싶은 데이터를 지운다.
	void Remove_WeaponBone(_uint iWeaponBoneIndex);
	// BeginTransform의 정보를 초기화 합니다.
	void Initialize_BeginTransform() { XMStoreFloat4x4(&m_vBeginTransformMatrix, XMMatrixIdentity()); }
	// 현재 재생중인 애니메이션의 이동값과 인자로 받아온 Distance를 비교하여 이동거리보다 긴지 짧은지를 
public:
#ifdef _DEBUG
	virtual HRESULT NativeConstruct_Prototype();
#endif // _DEBUG
	virtual HRESULT NativeConstruct_Prototype(_wstring strPath);
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	void Tick(const _double& _dTimeDelta);
	void Late_Tick(const _double& _dTimeDelta);

private:
	//	InputTrigger와 Transition이 갖고있는 InputTrigger를 비교하여 조건이 충족하면 다음 애니메이션 클립을 재생시킨다.
	void Change_AnimationClip();
	// 다음 애니메이션 클립을 설정한다.
	void Set_NextAnimClip(CAnimationClip* _pNextClip) { m_pNextClip = _pNextClip; }
	// 애니메이션 클립을 업데이트한다.
	void Update_AnimationClip(const _double& _dTimeDelta);
	// 자신에게 필요한 Trigger가 들어온 트랜지션들이 반복문을 돌며 조건처리를 하는 함수
	void Check_Transition_Condition();
public:
	// 애니메이션의 뼈값만 업데이트해준다. ( TimeDelta값을 0으로 처리 )
	void Update_InitBoneState();
	// 한 프레임 단위로 실행시킨다.
	void Play_OneFrame(_double TimeDelta);

private:
	// 애니메이터 클래스를 들고있는 객체의 트랜스폼 컴포넌트
	CTransform*				m_pOwnerTransformCom = nullptr;
	// 애니메이션 클립을 적용시킬 애니메이션 모델 컴포넌트
	CModel*					m_pModelCom = nullptr;
	// 현재 돌고있는 애니메이션 클립
	CAnimationClip*			m_pCurrentClip = nullptr;
	// 다음 들어올 애니메이션 클립
	CAnimationClip*			m_pNextClip = nullptr;
	// 다음 들어올 애니메이션 클립 ( 보간을 하지 않고 애니메이션이 끝났을 경우 Set을 이용하여 현재 클립으로 낀다. )
	CAnimationClip*			m_pNextDirectClip = nullptr;
	// 이전 애니메이션 클립
	CAnimationClip*			m_pPrevClip = nullptr;
	// 애니메이션 클립들
	vector<CAnimationClip*>	m_AnimationClips;
	// 트리거 리스트 ( string 문자열로 저장 )
	vector<_string>			m_TriggerList;
	// 현재 애니메이션 클립이 진행되는 동안 들어온 트리거 명령들
	vector<INPUTTRIGGER>	m_InputTriggerList;
	// Transition 목록
	vector<TRANSITION>		m_TransitionList;
	// 애니메이션 클립이 갖고있는 트리거와 같은 이름이 들어온 트랜지션들
	vector<TRANSITION>		m_ActiveTransitionList;
	// 무기를 붙일 뼈를 담아놓는 리스트
	vector<CHierarchyNode*> m_WeaponBoneList;
	// 애니메이션 클립이 변경됐는지 체크한다.
	_bool					m_bIsChange = false;
	// 애니메이션 클립을 진행시킬지 결정하는 bool값
	_bool					m_bIsPlay = false;
	// 애니메이션 변경할 때 사용할 Transition
	TRANSITION*				m_pCurTransition = nullptr;
	// Root본 애니메이션 사용할 때 사용할 본 이름
	_string					m_strRootBoneName = "Root";
	// Root본 애니메이션을 사용하기 위해 애니메이션이 바뀔때 현재 위치를 담아준다.
	_float4x4				m_vBeginTransformMatrix;
	// Root본 애니메이션이 끝날때 각도를 적용하기위해 존재하는 변수
	_float3					m_vRootBoneRotation = _float3(0.f, 0.f, 0.f);
	// Root본 Transform 애니메이션 ( 내가 지정한 이동값으로 이동하는것 )의 프레임당 이동값을 담는 변수
	_float4					m_vBoneTransformPos;
	// Root본 애니메이션으로 구해진 m_vRootBoneTransform + BoneDeltaPos 행렬
	_float4x4				m_vRootBoneMatrix;
	// Prev Root Bone matrix
	_float4x4				m_vPrevRootBoneMatrix;
	// Real Bone Position
	_float3					m_vRealBonePosition = _float3(0.f, 0.f, 0.f);
	// Root본 Transform 애니메이션 사용시 애니메이션의 위치를 조정하기위한 변수
	_float4					m_vAdjustPos;
	// 애니메이터를 가진 객체가 현재 진행하고있는 방향값
	_float3					m_vOwnerDirection = _float3(0.f, 0.f, 0.f);
	// 트랜지션 목록을 업데이트 할지에 대한 여부
	_bool					m_bIsTransitionUpdate = false;
public:
	static	CAnimator*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _wstring strPath);
	static	CAnimator*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	// Animator컴포넌트는 pArg에 모델 객체를 넣어주자
	virtual CComponent* Clone( void* pArg)override;
	virtual void Free() override;
};

END