#pragma once
#include "Character.h"

BEGIN(Engine)
class CCollider;
class CController;
class CAnimator;
class CAnimationClip;
END

BEGIN(Client)

class CPlayer : public CCharacter
{
public:
	typedef enum EControllerType {
		CONTROLLER_AI, CONTROLLER_KEYBOARD, CONTROLLER_GAMEPAD, CONTROLLER_END
	}CONTROLLER_TYPE;
	typedef struct tagPlayerData
	{
		EControllerType eControllerType = CONTROLLER_END;
	}PLAYERDATA;

protected:
	explicit CPlayer(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CPlayer(const CPlayer& _rhs);
	virtual ~CPlayer() = default;

public:
	CTransform* Get_TargetTransform() const { return m_pTargetTransform; }
	class CSword* Get_Weapon() const { return m_pSword; }

	// 오니 아이콘에서 오니 트랜스폼을 받아 타겟 트랜스폼으로 세팅하는 함수 
public:
	void Set_TargetTransform(CTransform* _TargetTransform) { m_pTargetTransform = _TargetTransform; };

public:
	virtual HRESULT		NativeConstruct_Prototype() override;
	// 클론할 때 Player Data를 집어 넣어야 한다!!
	virtual HRESULT		NativeConstruct_Clone(void* pArg) override;
	virtual void		Tick(const _double& _dTimeDelta) override;
	virtual void		Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	// Succes Dash
	void							Success_Dash();
	// Success Dash
	void							Check_SuccessDash();
	// Damaged
	void						Damaged(CPlayer* _pPlayer);
	// Coll Tick
	virtual void				Coll_Tick(_matrix _WorldMatrix) override;
public:
	// 성공한 공격의 종류를 받아서 해당 공격이 적중시 이뤄져야하는 행동등을 설정합니다. ( 해당 함수를 한번만 호출함. )
	virtual void	Check_Hit_Type(_string _strHitType, CCharacter* _pEnemyCharacter) override;
	// Play End Cut Scene 
	virtual void Play_EndCutScene(const _bool& _bIsAI = false);
protected:
	// Coll Init
	virtual void				Coll_Init() override;
	// Set Up Constant
	virtual HRESULT		SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT		SetUp_Components() override;
protected:
	// 피격 당했을때 처리
	virtual void		Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager);
	// 피격 당했을때 이펙트 처리
	virtual void		Hit_Effect(_string strColliderType, CCharacter* _pEnemyCharcter);
	// SetUp AI Controller
	virtual HRESULT		SetUp_Controller(const CONTROLLER_TYPE& _eType);
	// SetUp Weapon
	virtual HRESULT		SetUp_Weapon(FILEDATA* _pFileData, const _wstring& _strModelProtoName, const _string& _strModelStartBone,
		const _string& _strModelEndBone);
	// SetUp Sheath
	virtual HRESULT		SetUp_Sheath(FILEDATA* _pFileData, const _wstring& _strModelProtoName);
	// Datas
	virtual void	Fill_Datas();
	// Chnage State
	virtual void				Behavior_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Animation Controller State 
	virtual void				Animation_Control_State(const _double& _dTimeDelta, VOIDDATAS _Datas);

	// Trail 활성화 컨트롤
	virtual void Control_Trail_Active();
	// ImGui 연동
	virtual void Extern_ImGuiClass() override;

protected:
	// Skill 1
	virtual void	Skill_1(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Skill 2 
	virtual void	Skill_2(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Skill 3
	virtual void	Skill_3(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Move Normal
	virtual void	Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack
	virtual void	Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Upper
	virtual void	Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Down
	virtual void	Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Awake
	virtual void	Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Ultimae
	virtual void	Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Charge Attack
	virtual void	Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Step Back
	virtual void	Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Dash Ground
	virtual void	Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Dash Air
	virtual void	Dash_Air_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Jump
	virtual void	Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// 낙법
	virtual void	Passive_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// 구르기
	virtual void	Roll_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// 일반적인 Hit의 경우에 사용
	virtual void	Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// 공중에 떠서 Hit 됐을 경우에 사용
	virtual void	Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Upper맞아서 AIR_DOWN 상태가 됐거나 하는 경우에 사용
	virtual void	Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Strike ( 공중에서 STRIKE 공격에 맞은 경우에 사용 )
	virtual void	Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Bound 됐을때의 상태처리
	virtual void	Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// BLOW 됐을때의 상태처리 ( 스킬등에 맞아서 하늘에 붕 뜨는 경우 사용함. )
	virtual void	Blow_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;

public:
	// 성공한 공격의 종류를 받아서 해당 공격이 적중시 이뤄져야하는 행동등을 설정합니다. ( 해당 함수를 한번만 호출함. )
	virtual void	Check_Attack_Type(_string _strHitType) override;
protected:
	// Upper 공격에 성공했을 때 호출된다.
	virtual void	Success_Attack_Upper() override;
	// Down 공격에 성공했을 때 호출된다.
	virtual void	Success_Attack_Down() override;
	// 스킬 1 공격에 성공했을 때 호출된다.
	virtual void	Success_SKILL_1() override;
	// 스킬 2 공격에 성공했을 때 호출된다.
	virtual void	Success_SKILL_2() override;
	// 스킬 3 공격에 성공했을 때 호출된다.
	virtual void	Success_SKILL_3() override;
	// 잡기 공격에 성공했을 때 호출된다.
	virtual void	Success_Throw() override;
	// 개방 추가 공격에 성공했을 때 호출된다. ( 컷씬 실행 )
	virtual void	Success_Attack_Awake() override;
	// 오의 시동기를 맞췄을 때 호출된다. ( 컷씬 실행 )
	virtual void	Success_Ultimate() override;



protected:
	// 맵에 따른 네비게이션을 설정합니다.
	HRESULT Set_BattleMap_Navigation();

protected:
#ifdef _DEBUG
protected:
	virtual void				Coll_Render() override;
	virtual void				Coll_Renderer() override;
#endif
protected:
	// 공중대쉬 트리거 입력
	_bool		Input_AirDash_Trigger(const _string& _strTrigger);
	// 일반 대쉬 트리거 입력
	_bool		Input_Dash_Trigger(const _string& _strTrigger);

protected:
	void								Update_TargetTransform();
protected:
	// Enemy Type
	_uint							Get_EnemyType();
	// Enemy Dot
	_float							Get_EnemyToPlayerDot();
protected:
	// 움직일 방향을 받아옵니다.
	_vector							Get_MoveDirection();
	// 자신으로부터 상대방 객체까지의 Y축을 제외한 방향 벡터를 생성합니다.
	_vector							Get_DirToTarget();
	// 실행될 스텝의 방향 트리거를 받아옵니다.
	_string							Get_StepDir();
	// 스텝의 방향별 Look을 설정합니다.
	void							Set_StepLook(const _string& _strTrigger);

protected:
	// 기력게이지를 사용할 수 있는지 체크한다.
	_bool							Check_ChargeValue();
protected:
	// Sword
	class CSword* m_pSword = nullptr;
	// Sheath
	class CSheath* m_pSheath = nullptr;
	// Prev Collision
	class CCollider* m_pPrevCollider = nullptr;;
	// TargetTransform
	CTransform* m_pTargetTransform = nullptr;
	// Controller Type 
	CONTROLLER_TYPE						m_eControllerType = CONTROLLER_END;
	// Enemy State
	_string								m_strEnemyState = "";
	// Animation Step Clip
	CAnimationClip* m_pAnimStepClip = nullptr;
	// Dash Collision
	_bool								m_bDashCollision = false;
	// Charge Acc
	CUSTIMER							m_tChargeTimer = CUSTIMER(3.f);
	VOIDDATAS					m_InsertVoidDatas;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END