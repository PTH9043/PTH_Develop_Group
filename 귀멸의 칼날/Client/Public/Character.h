#pragma once
#include "Pawn.h"

BEGIN(Engine)
class CBonesCollider;
class CController;
class CAnimator;
class CAnimationClip;
class CModel;
class CNavigation;
END

BEGIN(Client)
class CBattle_Manager;
class CCutSceneModul;

class CCharacter : public CPawn
{
public:
	enum class CHAR_TYPE
	{
		AKAZA, SABITO, TANJIRO, ZENITSU, ONI, TEONI, CHAR_END
	};

	enum  INPUTDATA : _uint
	{
		INPUT_ANIMTYPE = 0, INPUT_TRANSITIONLIST,
		INPUT_CURANIMATION, INPUT_PREVANIMATION,

		INPUT_END
	}INPUTDATA;

	enum OUTDATA : _uint
	{
		OUT_STATE = 0, OUT_DIRECTION, OUT_TARGET_TRANSFORM,
		OUT_END
	};

	// KnockBack
	typedef struct tagKnockBack
	{
		// 애니메이션의 넉백이 적용되는 총 재생시간
		_double dPlayTime = 0.0;
		// 미는 힘
		_float	 fPushedForce = 0.f;
		// 포물선의 최대 높이 ( 없을경우 밀리기만 함. )
		_float  fHeight = 0.f;
		// 밀리는 방향 ( 때린 객체 -> 맞은 객체로 향하는 방향벡터 )
		_float3 vAttackDir = _float3(0.f, 0.f, 0.f);
	}KNOCKBACK;

	// Battle Data
	typedef struct tagBattleData
	{
		tagBattleData() = default;
		tagBattleData(const _float& _HP, const _float& _Power, const _float& _DefensiveValue,
			const _float& _AttackTimer = 20.f, const _float& _fComboTimer = 1.f)
			: fMaxHp(_HP), fCurHp(_HP), fAttackPower(_Power), fDefensiveValue(_DefensiveValue),
			tAttackTimer(CUSTIMER(_AttackTimer)), tComboTimer(CUSTIMER(_fComboTimer))
		{}
		void Decrease_HP(const _float& _fDamage);

		void Init();
		// Die 
		_bool Is_Die() { if (fCurHp <= 0.f) return true; return false; }
		// Increase
		void Increase_Charge(const _float& fValue) { fChargeValue += fValue; if (fChargeValue >= fMaxChargeValue) fChargeValue = fMaxChargeValue; }
		// Decrease Charge Value
		void Decrease_Charge(const _float& fValue);
		// Is Charget Max Charge
		_bool Is_CheckMaxCharge() { if (fMaxChargeValue == fChargeValue) return true; else return false; }
		// Increase
		void Increase_Ultimate(const _float& fValue);
		// Increase_UltimateCount
		void Increase_UltimateCount();
		// Full of UltimateCount
		_bool Is_CheckMaxUltimateCount() { if (iUltimateCount == iUltimateMaxCount) return true; else return false; }
		// Decrease AwakeValue
		void Decrease_AwakeValue(const _float& fValue);
		// Awake Active
		void Active_AwakeMode();
		// Add Win Count
		void Increase_WinCount() { iWinCount++; }
		// Increase HP
		void Increase_HP(const _float& fValue) { fCurHp += fValue; if (fCurHp >= fMaxHp) fCurHp = fMaxHp; }

		// Value
		_float				fMaxHp = 100.f;
		_float				fCurHp = 0.f;
		_float				fAttackPower = 0.f;
		_float				fDefensiveValue = 0.f;
		_float				fMaxChargeValue = 100.f;
		_float				fChargeValue = 0.f;
		_float				fMaxUltimateValue = 100.f;
		_float				fUltimateValue = 0.f;
		_float				fAwakeValue = 100.f;
		// Ultimate Count
		_uint				iUltimateMaxCount = 3;
		_uint				iUltimateCount = 0;
		// Hit Count
		_uint				iHitCount = 0;
		// Awake
		_bool				bIsAwake = false;
		// Hit Charge Timer
		CUSTIMER		tChargeTimer = CUSTIMER(3.f);
		// Hit Timer
		CUSTIMER		tAttackTimer = CUSTIMER(20.f);
		// Combo Timer
		CUSTIMER		tComboTimer = CUSTIMER(1.0f);
		// Win Count
		_uint			iWinCount = 0;
		// Stop
		_bool			bBattleStop = false;
		// Over Hit
		_bool			bIsOverHit = false;
	}BATTLEDATA;

protected:
	explicit CCharacter(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CCharacter(const CCharacter& _rhs);
	virtual ~CCharacter() = default;
public:
	// Battle Data
	BATTLEDATA& Get_BattleData() { return m_tBattleData; }
	// BoneCollider
	BONECOLLIDERS Get_BoneCollider() { return m_BonesCollider; }
	// Character Type
	const CHAR_TYPE& Get_CharType() const { return m_eCharType; }

	const _bool& Is_ActiveCutScene() const { return m_bActiveCutScene; }
public:
	CAnimator* Get_Animator() { return m_pAnimator; }
	// Controller
	CController* Get_Controller() { return m_pController; }
	// Animation Attack Collider
	CCollider* Get_AttackCollider() { return m_pAnimAttackCollider; }
	// Get Hit Check
	const _bool& Is_Hit() const { return m_bIsHit; }
	// Get Air Check
	const _bool& Is_Air() const { return m_bIsAir; }
	// Motion Blur
	const _bool& Is_MotionBlur() const { return m_bMotionBlur; }
	// Set Hit Check
	void Set_Hit(const _bool& bIsHit) { m_bIsHit = bIsHit; }
	// Set BehaviorDir
	void Set_BehaviorDir(const _vector& _vDir) { m_vBehaviorDir = _vDir; }
	// Set KnockBackInfo
	void Set_KnockBackInfo(const KNOCKBACK& _tKnockBack) { m_tKnockBackInfo = _tKnockBack; }

	void Set_ActiveCutScene(const _bool& _bIsActiveCutScene) { this->m_bActiveCutScene = _bIsActiveCutScene; }

	void Set_Navigation(const _tchar* pNavigationTag);


public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT	RenderLight() override;
	virtual void Extern_ImGuiClass() override;
	// Start Scene 
	void Start_Scene(CCharacter* _Enemy);
public:
	// OtherChracter To distance
	_float OtherCharacterToDistance(CGameObject* _pOther);
	_float OtherCharacterDirToLook(CGameObject* _pOther);
	_float OhterCharacterDirToRight(CGameObject* _pOther);
	_float OtherCharacterDirToLookConverter(CGameObject* _pOther);
	_float3 OtherCharacterDirToLookVectorF3(CGameObject* _pOther);

	// OtherChracter To distance
	_float OtherCharacterToDistance(CTransform* _pOther);
	_float OtherCharacterDirToLook(CTransform* _pOther);
	_float OhterCharacterDirToRight(CTransform* _pOther);
	_float OtherCharacterDirToLookConverter(CTransform* _pOther);
	_float3 OtherCharacterDirToLookVectorF3(CTransform* _pOther);
protected:
	// SetUp Cut Scnee 
	HRESULT Add_CutScene(FILEDATA* _pFileData);
protected:
	_uint	Judge_Direction(const _float& _fDot);
	// 현재 플레이어가 움직이는지 체크
	_bool	Is_JudgeMoveState();
public:
	// 현재 떨어지는 상태 인지 체크
	_bool	Is_JudgeFallState();
	// 충돌된 만큼 밀어준다.
	virtual void	Push(CCharacter* _pOther);
	// 내 공격이 상대방에게 맞았을 때 상태 처리 
	virtual void	Check_AttackCollision(CCharacter* _pOther);
	// 투사체에 맞았을 경우
	virtual void	Hit_ByProjectile(_float _fPushForce, _float _fHeight, _float _fDamage, _float _fDelayTime, CCharacter* _pEnemyCharcter);
	// 공격에 맞은 상대의 상태를 검사하여 콜라이더 체크를 할지 말지 결정한다.
	_bool			Check_Enemy_State(CCharacter* _pEnemyCharcter);

protected:
	// AnimAttackCollider에 맞았을 경우
	virtual void	Success_AnimAttack(CCharacter* _pOther);
	// 피격 당했을때 처리
	virtual void	Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager);
	// 피격 당했을때 이펙트 처리
	virtual void	Hit_Effect(_string strColliderType, CCharacter* _pEnemyCharcter);
	// 밀치기 공격에 맞았을때의 처리
	virtual void	Hit_Pushed(_vector _vDir);
	// 슈퍼아머 상태로, 맞았지만 데미지만 들어오고 밀리거나 하지 않는 경우를 판단한다.
	virtual _bool	Hit_SuperArmor();

	// SetUp Animator
	virtual HRESULT SetUp_Animator(const _tchar* _pName, CModel* _pModel);
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;
	// Jump
	virtual void Jump(const _double& _dTimeDelta, const _string& _strTrigger);
	// Charge
	virtual void Charge(const _double& _dTimeDelta, const _string& _strTrigger);

	// 프레임당 이동값 계산 함수 ( MoveSpeed를 이용하여 구함. )
	_vector Compute_MovePos(const _double& _dTimeDelta, _vector vMoveDir, _float fMoveSpeed, _float fMaxHeight = 0.f, _float fMaxHeightRatio = 0.f, _float fPlayTime = 0.f);
	// 프레임당 이동값 게산 함수 ( 최대 이동할 거리를 이용하여 구함. )
	_vector Compute_MovePos_ByDist(const _double& _dTimeDelta, _vector vMoveDir, _float fDist, _float fMaxHeight = 0.f, _float fMaxHeightRatio = 0.f, _float fPlayTime = 0.f);
	// 포지션을 강제로 아래로 내리게 만드는 함수
	_vector Strike(const _double& _dTimeDelta, _vector vMoveDir, _float fDist, _float fMaxHeight = 0.f, _float fMaxHeightRatio = 0.f, _float fPlayTime = 0.f);
	// Bound
	virtual void Bound(const _double& _dTimeDelta);
	// Fall
	virtual void Fall(const _double& _dTimeDelta, const _string& _strTrigger);
	// Restrict Nav
	virtual void Restrict_Nav(const _double& _dTimeDelta);

protected:
	// Skill 1
	virtual void	Skill_1(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Skill 2 
	virtual void	Skill_2(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Skill 3
	virtual void	Skill_3(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Move Normal
	virtual void	Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Attack
	virtual void	Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Attack Upper
	virtual void	Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Attack Down
	virtual void	Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Attack Awake
	virtual void	Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Ultimae
	virtual void	Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Charge Attack
	virtual void	Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Step Back
	virtual void	Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Dash Ground
	virtual void	Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Dash Air
	virtual void	Dash_Air_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Jump
	virtual void	Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// 낙법
	virtual void	Passive_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// 구르기
	virtual void	Roll_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// 일반적인 Hit의 경우에 사용
	virtual void	Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// 공중에 떠서 Hit 됐을 경우에 사용
	virtual void	Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Down 되어있는 상태일때의 상태 처리
	virtual void	Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Upper맞아서 AIR_DOWN 상태가 됐거나 하는 경우에 사용
	virtual void	Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Strike ( 공중에서 STRIKE 공격에 맞은 경우에 사용 )
	virtual void	Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Bound 됐을때의 상태처리
	virtual void	Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Spin 됐을때의 상태처리 ( Spin 도중에는 무적임 )
	virtual void	Spin_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// BLOW 됐을때의 상태처리 ( 스킬등에 맞아서 하늘에 붕 뜨는 경우 사용함. )
	virtual void	Blow_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Push에 맞았을때의 상태처리
	virtual void	Hit_PushedState(const _double& _dTimeDelta);
	// Dash끼리 부딫혔을때의 상태처리
	virtual void	Dash_Reflect_State(const _double& _dTimeDelta);
	// DEAD 상태일떄의 상태처리
	virtual void	Dead_State(const _double& _dTimeDelta);

public:
	// 성공한 공격의 종류를 받아서 해당 공격이 적중시 이뤄져야하는 행동등을 설정합니다. ( 해당 함수를 한번만 호출함. )
	virtual void	Check_Attack_Type(_string _strAttackType);
	// 맞은 공격의 종류를 받아서 해당 공격을 맞았을때의 이뤄져야하는 작업을 설정합니다.
	virtual void	Check_Hit_Type(_string _strHitType, CCharacter* _pEnemyCharcter);
protected:
	// Upper 공격에 성공했을 때 호출된다.
	virtual void	Success_Attack_Upper();
	// Down 공격에 성공했을 때 호출된다.
	virtual void	Success_Attack_Down();
	// 스킬 1 공격에 성공했을 때 호출된다.
	virtual void	Success_SKILL_1();
	// 스킬 2 공격에 성공했을 때 호출된다.
	virtual void	Success_SKILL_2();
	// 스킬 3 공격에 성공했을 때 호출된다.
	virtual void	Success_SKILL_3();
	// 잡기 공격에 성공했을 때 호출된다.
	virtual void	Success_Throw();
	// 개방 추가 공격에 성공했을 때 호출된다. ( 컷씬 실행 )
	virtual void	Success_Attack_Awake();
	// 오의 시동기를 맞췄을 때 호출된다. ( 컷씬 실행 )
	virtual void	Success_Ultimate();

protected:
	// Coll Init
	virtual void Coll_Init() override;
	// Coll Tick
	virtual void Coll_Tick(_matrix _WorldMatrix) override;
#ifdef _DEBUG
	// Render에서 그려줄때
	virtual void Coll_Render() override;
	// Renderer 컴포넌트에 넣어줘서 그려줄때
	virtual void Coll_Renderer() override;
#endif
	// Bones Data 
	void Load_BonesData();
	// Input Trigger 
	void Input_Trigger(const _string& _strCurrentTrigger);
	// 대쉬 성공 판단
	void Success_Dash(CCharacter* _pOther, CBattle_Manager* _pBattleManager);

protected:
	// 사운드 추가 함수들을 사용하는 함수
	virtual HRESULT Initialize_Sound();
	// 사운드를 추가합니다. ( 1인자 = 등록할 키값, 2인자 = 사운드 파일 이름 )
	HRESULT Add_Sound(const _tchar* _szKey, const _tchar* _szFileName);
	// 등록된 사운드 리스트를 받아옵니다.
	vector<const _tchar*>* Find_SoundList(const _tchar* _szKey);
	// 등록된 사운드들을 랜덤으로 재생합니다.
	HRESULT Play_RandomSound(const _tchar* _szKey, _float fVolume = 0.5f);
	// 등록된 사운드들을 랜덤으로 재생합니다 ( 중복 체크 )
	HRESULT Play_RandomSound_ByCondition(const _tchar* _szKey, _float fVolume = 0.5f);
	// 등록된 사운드들을 그냥 재생합니다.
	HRESULT Play_Sound(const _tchar* _szKey, const _tchar* _szFileName, _float fVolume = 0.5f);

protected:
	// Controller
	CController* m_pController = nullptr;
	// Animator 
	CAnimator* m_pAnimator = nullptr;
	// Collider
	CCollider* m_pAttackColliderCom = nullptr;
	// Collider											
	CCollider* m_pAnimAttackCollider = nullptr;
	// Navigation
	CNavigation* m_pNavigationCom = nullptr;
	// Current Animation Clip 
	CAnimationClip* m_pCurrentAnimationClip = nullptr;
	// Cut Scene Modul 
	map<_string, CCutSceneModul*>	m_CutSceneModulMap;
protected:
	// Previouse Position
	_float3													m_vPreviousPosition = _float3(0.f, 0.f, 0.f);
	// Bones Collider
	BONECOLLIDERS											m_BonesCollider;
	// Collision Hit Frame
	_bool													m_bDamagedToCollision = false;
	// 밀리는 힘
	_float													m_fPushedForce = 0.f;
	// Char Type
	CHAR_TYPE												m_eCharType = CHAR_TYPE::CHAR_END;
	// State 
	_uint													m_iState = 0;
	// Move Speed
	_float													m_fMoveSpeed = 0.f;
	// Save Move Speed
	_float													m_fSaveMoveSpeed = 0.f;
	// Rotate Speed
	_float													m_fRotateSpeed = 0.f;
	// Camera의 Right Up Look 기준으로 만들어진 플레이어가 움직이는 경우에 사용하는 방향변수
	_float3													m_vDirection = _float3(0.f, 0.f, 0.f);
	// 점프나 대쉬할 경우 애니메이션이 시작될때 처음으로 들어가는 방향값 ( 이 방향값으로 애니메이션이 끝날때까지 사용 )
	_float3													m_vBehaviorDir = _float3(0.f, 0.f, 0.f);
	// Pushed Pos (밀리는 위치)
	_float3													m_vPushedPos = _float3(0.f, 0.f, 0.f);
	// 중력
	_float													m_fGravity = 0.f;
	// TimeAcc
	_double													m_dJumpTimeAcc = 0.0;
	// Current Trigger
	_string													m_strCurrentTrigger = "";
	// Prev Trigger
	_string													m_strPrevTrigger = "";
	// Jump Height
	_float													m_fCurrentJumpHeight = 0.f;
	// Prev Jump Height
	_float													m_fPrevJumpHeight = 0.f;
	// Jump Speed
	const _float											m_fJumpSpeed = 120.f;
	// Damaged Count
	_uint													m_iDamgedCount = 0;
	// Collider Direction
	_float4													m_vColliderDir = _float4(0.f, 0.f, 0.f, 0.f);
	// Hit Check
	_bool													m_bIsHit = false;
	// Check Push
	_bool													m_bIsHitPush = false;
	// Push TimeAcc
	_double													m_dPushTimeAcc = 0.0;
	// KnockBack 정보
	KNOCKBACK												m_tKnockBackInfo;
	// 공중에 떠있는지를 알려주는 불변수
	_bool													m_bIsAir = false;
	// 추락 시간 Acc
	_double													m_dFallTimeAcc = 0.0;
	// 스킬 게이지
	_float													m_fSkillGauge = 0.f;
	// 기력 게이지
	_float													m_fStamina = 0.f;
	// 스킬 게이지 카운트
	_uint													m_iSkillCount = 0;
	// 차지 공격 카운트
	CUSTIMER												m_tChargeAttackTimer = CUSTIMER(2.f);
	// 현재 재생중인 애니메이션 클립의 타입
	_string													m_strAnimType = "";
	// Battle Data
	BATTLEDATA												m_tBattleData;
	// 모델 빛나는 색상
	_float4													m_vGlowColor = _float4(1.f, 1.f, 1.f, 1.f);
	// 외각선 빛나는 색상
	_float4													m_vEdgeGlowColor = _float4(0.f, 0.f, 0.f, 0.f);
	// 대쉬 성공의 여부 ( 대쉬 성공시 캐릭터들의 움직임과 애니메이터의 Tick을 멈춘다. )
	_bool													m_bIsSuccessDash = false;
	// Object Motion Blur
	_bool													m_bObjectMotionBlur = false;
	_bool													m_bActiveCutScene = false;
	// 대쉬끼리 충돌 시 만들어지는 방향벡터
	_float4													m_vDashReflectDir = _float4(0.f, 0.f, 0.f, 0.f);
	_float													m_fDashReflectDistance = 30.f;
	// 개방 이펙트를 종료하기 위한 변수
	CGameObject* m_pAwakeEffect = nullptr;
	// 개방 이펙트 호출 타이머
	_float m_fAwakeEffectTimer = 0.f;

	// 사운드 맵 리스트
	map<const _tchar*, vector<const _tchar*>>				m_SoundList;

	_bool													m_bMotionBlur = false;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END