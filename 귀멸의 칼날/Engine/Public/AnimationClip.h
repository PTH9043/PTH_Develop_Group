#pragma once
#include "Engine_Defines.h"
#include "Base.h"

// 애니메이션 클립 클래스의 경우에는 메인 애니메이션과 서브 애니메이션을 갖고 있으며, 메인 애니메이션의 진행도에 맞춰
// 다양한 종류의 이벤트들을 발생시킨다.
// Animator 클래스에서 AnimationClip들을 등록해주면 vector에 넣어 관리한다.

BEGIN(Engine)
class CAnimation;
class CChannel;
class CHierarchyNode;

class ENGINE_DLL CAnimationClip final : public CBase
{
public:
	// 이벤트 타입
	enum ANIMEVENTTYPE
	{
		// 이펙트, UI, 사운드,카메라, 트리거  등등 필요시 추가예정
		EVENTTYPE_EFFECT,			// 이펙트
		EVENTTYPE_UI,				// UI
		EVENTTYPE_SOUND,			// SOUND
		EVENTTYPE_COLLIDER,			// COLLIDER
		EVENTTYPE_WEAPONBONE,		// WEAPONBONE
		EVENTTYPE_CAMERA,			// CAMERA
		EVENTTYPE_MESH,				// 특정 메쉬 숨기기
		EVENTTYPE_SPEEDSECTION,		// 애니메이션 재생의 특정 구간동안 속도를 조절하는 이벤트
		EVENTTYPE_BONETRANSFORM,	// 애니메이션 재생의 특정 구간동안 뼈의 위치값을 조절하는 이벤트		
		EVENTTYPE_GRAVITY,			// 애니메이션 재생의 특정 구간동안 중력값을 받을지 안받을지 결정하는 이벤트		
		EVENTTYPE_TIMECHECK,		// 애니메이션에서 특정 시간을 구하기 위한 이벤트		
		EVENTTYPE_TRAIL,			// 트레일이 있는경우 트레일을 껐다 켰다 할 수 있는 이벤트 
		EVENTTYPE_END
	};

	// 애니메이션이 갖고있는 이벤트 정보 ( Enum으로 이벤트별 필요한 정보를 사용한다. )
	typedef struct tagAnimEventDesc
	{
		// 이벤트 유형
		ANIMEVENTTYPE	tEventType = EVENTTYPE_END;
		// 이벤트가 시작되는 시간 ( 애니메이션의 진행도 ) 0 - 1 사이의 값으로 결정
		_double		dStartRatio = 0.0;
		// 이벤트가 끝나는 시간 ( 애니메이션의 진행도 ) 0 - 1 사이의 값으로 결정 -> 쓸 필요 없는경우 0.0 으로 놓기
		_double		dEndRatio = 0.0;
		// 이벤트를 끝낼 시간 ( 시작되는 시간부터 끝나는 시간을 사용하고 싶을 때 )
		_double		dEndTime = 0.0;
		// 속도값 등 여러가지 값들을 사용할 때 사용할 Value값 ( 무기뼈 인덱스 등에도 사용가능 )
		_float		fValue = 0.f;
		// 이벤트의 이름
		_string		strName = "";
		// 속도값 등 여러가지 값들을 사용할 때 사용할 Value값 ( fValue로 부족할 경우에 사용함. )
		_float		fAddValue = 0.f;
		// 실행됐는지에 대한 확인 여부등을 알리는 bool변수
		_bool		bIsFinish = false;
		// 이벤트 TimeAcc값
		_double		dEventTimeAcc = 0.0;
		// 메쉬 인덱스들을 담아놓는 벡터 컨테이너 ( 특정 메쉬를 안보이게 하거나 하는 경우에 사용한다. )
		//vector<_uint> MeshIndexList;
		// 사용할 뼈의 이름이나 이벤트가 사용할 수 있는 string값을 저장한다.
		_string		strEventText = "";
		// 생기게 할 위치 ( 객체 생성 위치 -> 뼈를 사용하면 뼈 위치값에서 + 한 위치에서 나오게 됨. )
		_float4		vEventPos = _float4(0.f, 0.f, 0.f, 1.f);
		// 이벤트가 들고있는 Dir값
		_float4		vEventDir = _float4(0.f, 0.f, 0.f, 1.f);
		// 이벤트가 들고있는 각종 value를 입력 가능한 float4
		_float4		vEventValues = _float4(0.f, 0.f, 0.f, 1.f);
	}ANIMEVENTDESC;

	// 이벤트 실행 여부등을 체크할 때 정보를 담는 구조체
	typedef struct tagEventCheckDesc
	{
		_string		strEventName = "";
		_bool		bIsActive = false;
	}EVENTCHECKDESC;

private:
	explicit CAnimationClip();
	explicit CAnimationClip(const CAnimationClip& _rhs);
	virtual ~CAnimationClip() = default;

public:
	// 현재 클립에 등록된 Main애니메이션 클립을 반환합니다.
	CAnimation* Get_MainAnimation() { return m_pMainAnimation; }
	// 현재 클립에 등록된 Sub애니메이션 클립을 반환합니다.
	CAnimation* Get_SubAnimation() { return m_pSubAnimation; }
	// 현재 클립에 등록된 이벤트 리스트를 반환합니다.
	vector<ANIMEVENTDESC> Get_EventList() { return m_EventList; }
	// 메인 애니메이션 이름을 받아옵니다.
	const _string& Get_MainAnimationName();
	// 서브 애니메이션 이름을 받아옵니다.
	 const _string& Get_SubAnimationName();
	// 애니메이션 클립의 이름을 받아옵니다.
	const _string& Get_Name() { return m_strName; }
	// 루트본의 이름을 받아옵니다.
	_string Get_RootBoneName();
	// 애니메이션의 타입을 받아옵니다.
	const _string& Get_AnimationType() { return m_strAnimType; }
	// 메인 애니메이션의 Duration값을 받아옵니다.
	const _double& Get_Duration();
	// 메인 애니메이션의 Tps값을 받아옵니다.
	const _double& Get_TickPerSecond();
	// 메인 애니메이션의 TimeAcc값을 받아옵니다.
	const _double& Get_TimeAcc();
	// 메인 애니메이션의 이벤트 진행시간을 실제 시간으로 받아옵니다 ( 실제시간 => 이벤트 진행이 되는 Ratio *  Duration / TickPerSecond )
	const _double& Get_EventPlayTime();
	// 애니메이션 클립의 재생 속도를 받아옵니다.
	const _float& Get_AnimationSpeed() { return m_fSpeed; }
	// 현재 애니메이션의 RootBone 이동값을 받아옵니다.
	_matrix Get_RootBoneMatrix();
	// 현재 애니메이션의 RootTransform 이동값을 받아옵니다.
	_vector Get_RootTransform();	
	// WeaponBoneIndex를 리턴한다.
	const _uint& Get_WeaponBoneIndex() { return m_iWeaponBoneIndex; }	
	// 주인객체의 트랜스폼을 받아온다.
	CTransform* Get_OwnerTransform() { return m_pOwnerTransform; }
	// 콜라이더가 가진 방향값을 반환합니다.
	_vector Get_ColliderDir() { return XMLoadFloat4(&m_vColliderDir); }
	// 진행도를 변경한다.
	void Set_Ratio(_double dRatio);
	// 애니메이션들의 TimeAcc값을 변경한다.
	void Set_TimeAcc(_double dTimeAcc);
	// 메인 애니메이션을 세팅해준다.
	void Set_MainAnimation(CAnimation* _pAnimation); 
	// 서브 애니메이션을 세팅해준다.
	void Set_SubAnimation(CAnimation* _pAnimation);
	// 현재 애니메이션 클립이 루프중인지에 대한 여부를 반환합니다.
	const _bool& Is_Loop() { return m_isLoop; }
	// 현재 애니메이션이 끝났는지에 대한 여부를 반환합니다.
	_bool Is_Finish();
	// RootBone 적용을 할지에 대한 여부를 반환합니다.
	_bool Is_RootBone();
	// BoneTransform 적용을 할지에 대한 여부를 반환합니다.
	_bool Is_BoneTransform() { return m_bIsBoneTransform; }
	// 중력을 적용시킬지 안시킬지에 대한 여부를 반환합니다.
	_bool Is_Gravity() { return m_bIsGravity; }	
	// 메쉬를 끌지 켤지에 대한 여부를 반환합니다.
	_bool Is_MeshActive() { return m_bIsMeshActive; }
	// 트레일의 활성화 여부를 반환합니다.
	_bool Is_TrailActive() { return m_bIsTrailActive; }
	// 트레일 Value를 받아옵니다.
	_float2 Get_TrailValues();
	// TimeCheck 이벤트를 통해 활성화된 현재 이벤트를 반환합니다.
	const EVENTCHECKDESC& Get_EventCheckDesc() { return m_tEventCheck; }
	// BoneTransform의 이동값 제한을 합니다.
	const _float& Get_MoveDistance() { return m_fMoveDistance; }
	//  Bone Move Force
	const _float& Get_BoneMoveForce() const { return m_fBoneAnimMoveForce; }
	// 공격 콜라이더 활성화 여부를 지정합니다.
	const _bool& Is_AttackColliderActive() const { return m_bIsColliderActive; }
	// 콜라이더가 가진 위치값을 반환합니다.
	_vector Get_ColliderPos() { return XMLoadFloat4(&m_vColliderPos); }
	// 콜라이더가 가진 Scale값을 반환합니다.
	_vector Get_ColliderScale() { return XMLoadFloat4(&m_vColliderScale); }
	// 콜라이더가 가진 콜라이더 종류를 반환합니다.
	const _string& Get_ColliderType() { return m_strColliderType; }

	// 밀리는 힘을 받아옵니다.
	const _float& Get_PushedForce() const { return m_fPushedForce; }
	// 띄우는 힘을 받아옵니다 ( Y값 )
	const _float& Get_Height() const { return m_fHeight; }
	// 이벤트가 진행되는지를 받아옵니다.
	const _bool& Is_EventPlay() const { return m_bIsEventPlay; }

	// 선형보간 진행의 여부를 받아옵니다.
	_bool Is_SupplementSituation();
	// 선형보간 진행을 알려주는 bool값을 설정합니다.
	void Set_SupplementSituation(const _bool& _bSituation);
	// 애니메이션 클립의 이름을 변경합니다.
	void Set_Name(const _string& _strName) { m_strName = _strName; }
	// 루프 여부를 세팅합니다.
	void Set_Loop(const _bool& isLoop) { m_isLoop = isLoop; }
	// 현재 애니메이터가 가진 Model의 HierarchyNodeList를 세팅합니다.
	void Set_HierarchyNodeList(vector<CHierarchyNode*> _HierarchyNodeList) { m_HierarchyNodeList = _HierarchyNodeList; }
	// RootBone 여부를 세팅합니다.
	void Set_RootBone(const _bool& isRootBone);
	// RootBone의 이름을 Animation으로 보냅니다.
	void Set_RootBoneName(const _string& _strRootBoneName);
	// RootTransform에 필요한 애니메이터 컴포넌트를 들고있는 객체의 Look 벡터를 설정합니다.
	void Set_RootTransformDir(const _fvector _vLook) { XMStoreFloat3(&m_vOwnerLook, _vLook); }
	// 애니메이션의 타입을 설정합니다.
	void Set_AnimationType(const _string& _strAnimType) { m_strAnimType = _strAnimType; }
	// Finish 여부를 세팅합니다
	void Set_Finish(const _bool& isFinish);
	// 이벤트 리스트를 세팅한다
	void Set_Event(const vector<ANIMEVENTDESC>& _EventList) { m_EventList = _EventList; }
	// 애니메이션 속도값을 변경한다.
	void Set_AnimationSpeed(const _float& fFastValue);
	// OwnerTransform을 설정한다.
	void Set_OwnerTransform(CTransform* _pOwnerTransform);
	// OwnerDirection을 설정한다.
	void Set_OwnerDirection(_vector _vDir) { XMStoreFloat3(&m_vOwnerDirection,_vDir); }
	// RootBone의 Look을 담아준다.
	void Set_RootBoneLook(_vector _vRootBoneLook) { XMStoreFloat3(&m_vRootBoneLook, _vRootBoneLook); }
	// Ratio 값을 본다
	_double Get_Ratio();
	// 애니메이션들의 데이터를 초기화한다.
	void Reset_AnimationData();
	// 타겟과의 거리를 받아옵니다.
	void Set_TargetDistance(const _float& _fTargetDistance) { m_fTargetDistance = _fTargetDistance; }
	// BoneTransform 애니메이션의 경우 이동거리를 계산합니다. ( 제한을 두고싶은 경우 사용 )
	_bool Compute_Distance();

	void Set_SupplementAnim(const _bool& _bIsSupplementAnim) { this->m_bSupplementAnimation = _bIsSupplementAnim; }
public:
	virtual HRESULT NativeConstruct();
	virtual HRESULT NativeConstruct_Clone(void* pArg);
	virtual HRESULT NativeConstruct(CAnimation* pMainAnimation, CAnimation* pSubAnimation, const vector<ANIMEVENTDESC>& EventList, _string strName);
	void  Tick(const _double& _dTimeDelta);
	void  Late_Tick(const _double& _dTimeDelta);
	
public:
	// 애니메이터에서 클립이 바뀔때 클립의 설정을 초기화해줍니다.
	void Initialize_Data();
	// 다음 애니메이션으로 선형보간한다.
	void Update_NextAnimation(const _double& _dTimeDelta, CAnimationClip* _pNextClip);
	// 다음 애니메이션으로 선형보간한다. ( 0.0이 아닌 인자로들어오는 비율부터 시작 )
	void Update_NextAnimation(const _double& _dTimeDelta,const _double& _dNextRatio,const _double& _dLerpSpeed, CAnimationClip* _pNextClip);
	// Transform Matrices를 업데이트 한다.
	void Update_TransformMatrices(const _double& _dTimeDelta);
	// Transform Matrices를 업데이트 한다.
	void Update_TransformMatrices();

private:
	// string값으로 하이어라키 노드를 찾아옵니다.
	CHierarchyNode* Find_HierarchyNode(const _string& _strBoneName);

private:
	// 이벤트들의 업데이트를 한다. ( 이벤트 진행등의 작업이 이루어진다. )
	void Update_Event(const _double& _dTimeDelta);
	void Update_Event_Collider(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_Sound(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_Effect(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_MeshHide(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_SpeedSection(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_WeaponBone(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_RootTransform(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_Gravity(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_TimeCheck(ANIMEVENTDESC& _event, const _double& _dTimeDelta);	
	void Update_Event_Trail(ANIMEVENTDESC& _event, const _double& _dTimeDelta);

private:
	// 애니메이션 클립의 이름 -> Default는 애니메이션의 이름이지만 원한다면 애니메이터에서 변경해줄 수 있음.
	_string m_strName = "";
	// 애니메이션 타입 -> 현재 재생중인 애니메이션 클립이 공격, 스킬, 점프, 이동등의 명령들중에 어떤것에 해당되는지를 판단한다.
	_string m_strAnimType = " ";
private:
	// 메인 애니메이션
	CAnimation* m_pMainAnimation = nullptr;
	// 서브 애니메이션 -> 서브 애니메이션은 메인 애니메이션의 Update_Matrices가 모두 이루어진 후 한번 더 뼈를 업데이트함.
	CAnimation* m_pSubAnimation = nullptr;
	// 애니메이터에서 전달받은 HierarchyNodeList
	vector<CHierarchyNode*> m_HierarchyNodeList;
	// 메인 애니메이션의 영향을 받는 채널
	vector<CChannel*>	m_MainChannelList;
	// 서브 애니메이션의 영향을 받는 채널
	vector<CChannel*>	m_SubChannelList;
	// 애니메이션 기본 재생 속도
	_float					m_fSpeed = 1.f;
	// 이벤트 목록
	vector<ANIMEVENTDESC>	m_EventList;
	// Loop 여부를 담는 불값
	_bool					m_isLoop = false;
	// BoneTransform을 사용하는지에 대한 여부를 담는 불값
	_bool					m_bIsBoneTransform = false;
	// WeaponBoneIndex
	_uint					m_iWeaponBoneIndex = 0;
	// ColliderActive
	_bool					m_bIsColliderActive = false;
	// Collider에서 사용하는 Dir값
	_float4					m_vColliderDir = _float4(0.f, 0.f, 0.f, 0.f);
	// Collider Scale
	_float4					m_vColliderScale = _float4(0.f, 0.f, 0.f, 0.f);
	// Collider Position
	_float4					m_vColliderPos = _float4(0.f, 0.f, 0.f, 0.f);
	// PushedForce ( 콜라이더 충돌시 밀리는 힘 )
	_float					m_fPushedForce = 0.f;
	// Height ( 콜라이더 충돌시 띄우는 힘 )
	_float					m_fHeight = 0.f;
	// RootTransform을 사용할때 사용할 애니메이터 컴포넌트를 들고있는 객체의 Transform
	CTransform*				m_pOwnerTransform = nullptr;
	// RootTransform을 사용할때 사용할 애니메이터 컴포넌트를 들고있는 객체의 Look 벡터
	_float3					m_vOwnerLook = _float3(0.f, 0.f, 0.f);
	// RootTransform을 사용할때 사용할 애니메이터 컴포넌트를 들고있는 객체의 이동 벡터
	_float3					m_vOwnerDirection = _float3(0.f, 0.f, 0.f);
	// Root본의 Look
	_float3					m_vRootBoneLook = _float3(0.f, 0.f, 0.f);
	// Root본 애니메이션을 사용하기 위해 애니메이션이 바뀔때 현재 위치를 담아준다.
	_float4					m_vBeginTransform = _float4(0.f, 0.f, 0.f, 0.f);
	// Bone Anim Move Force
	_float					m_fBoneAnimMoveForce = 0.f;
	// 활성화되는 메쉬의 넘버를 담는다. ( size가 0이라면 모든 메쉬를 다 렌더한다. )
	vector<_uint>			m_MeshIndexList;
	// 애니메이션의 RootTransform 이벤트가 진행되는 비율
	_double					m_dEventPlayTime = 0.0;
	// 남은 애니메이션 RootTransform 진행시간
	_double					m_dRestPlayTime = 0.0;	
	// 애니메이션의 RootTransform 이벤트가 진행되는지 체크하는 불값
	_bool					m_bIsEventPlay = false;
	// 애니메이션에 이벤트에 등록되어있는 이동거리값
	_float					m_fMoveDistance = 0.f;
	// 타겟이 있다면 타겟과의 거리를 받아옵니다.
	_float					m_fTargetDistance = 0.f;
	// 트레일의 시작값
	_float					m_fTrailStartValue = 0.f;
	// 트레일의 끝값
	_float					m_fTrailEndValue = 0.f;
	// 중력값에 대한 여부를 받아옵니다.
	_bool					m_bIsGravity = true;
	// 메쉬 활성화에 대한 여부를 받아옵니다.
	_bool					m_bIsMeshActive = true;
	// 트레일 활성화 여부를 받아옵니다.
	_bool					m_bIsTrailActive = false;
	// 이벤트에서 생성된 콜라이더의 타입.
	_string					m_strColliderType = "";
	// TimeCheck 이벤트에서 활성화된 이벤트를 담는 구조체 변수 
	EVENTCHECKDESC			m_tEventCheck;
	// Supplement Animation
	_bool					m_bSupplementAnimation = true;
	// 사운드 랜덤 인덱스
	_int					m_iRandomSoundIndex = 0;
	// 랜덤 사운드 실행 여부 ( 한번만 되도록 )
	_bool					m_bIsRandomPlay = false;
	_string				STR_NULL = " ";
public:
	// 인자값으로 이벤트 목록을 받는다.
	static CAnimationClip* Create(CAnimation* pMainAnimation, CAnimation* pSubAnimation, const vector<ANIMEVENTDESC>& EventList, _string strName);
	static CAnimationClip* Create();
	CAnimationClip* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END