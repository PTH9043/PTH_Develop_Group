#pragma once
#include "Client_Defines.h"
#include "AI_CharController.h"
#include "Sabito.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CAIController_Sabito final : public CAI_CharController
{
public:
	enum DISTANCE_VALUE
	{
		DISTANCE_DASH_CANCEL_RANGLE = 10,

		DISTANCE_ATK_RANGE = 30, 

		DISTANCE_CHARGE_RANGE = 40,

		ATK_DISTANCE_END
	};

	typedef struct tagRandomPatternTIme
	{
		_double		dTime = 0.0;
		_uint			iValue = 0;
	}RANPATTIME;

	typedef vector<CSabito::STATE> PATTERNM_LIST;

protected:
	explicit CAIController_Sabito(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CAIController_Sabito() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
public:
	// 데이터를 내부에 집어 넣는 함수
	virtual void Insert_Datas(VOIDDATAS _pData);
	// 데이터를 바깥으로 빼는 함수
	virtual  VOIDDATAS Out_Datas();
protected:
	// Judge State
	virtual void Judge_State(const _double& _dTimeDelta) override;
protected:
	// IDLE
	void Current_Idle_state(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Idle To Enemy Idle Pattern 1
	void CurIdle_To_EnemyIdle_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, 
		class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Idle To Enemy Move Pattern  
	void CurIdle_To_EnemyMove_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Idle To Enemy Attack Pattern 
	void CurIdle_To_EnemyAttack_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Idle To Enemy Gaurd Pattern
	void CurIdle_To_EnemyGuard_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Idle To enemy Sp Attack Pattern
	void CurIdle_To_EnemyJump_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Idle To Enemy Hit Pattern
	void CurIdle_To_EnemyHit_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Cur Idle To Pass Time 
	void	 CurIdle_To_PassTime(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected:
	// Current Move State
	void Current_Move_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Cur Move To Enemy Idle Pattern
	void CurMove_To_EnemyIdle_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Cur Move To Enemy Move Pattern
	void CurMove_To_EnemyMove_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Cur Move To Enemy Attack Pattern
	void CurMove_To_EnemyAttack_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Cur Move To Enemy Guard Pattern
	void CurMove_To_EnemyGaurd_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Cur Move To Enemy Jump Pattern
	void CurMove_To_EnemyJump_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Cur Move Enemy Hit Pattenr
	void CurMove_To_EnemyHit_Pattern(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected:
	// Current Dash State
	void Current_Dash_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected:
	// Jump Dash State
	void Current_JumpDash_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected: /*  Attack  */
		// Current Move State
	void Current_Attack_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Current Attack S State
	void Current_Attack_S_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Skill
	void Current_Skill_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Jump Attack
	void Current_JumpAttack_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected:
	// Current step State 
	void Current_Step_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected:
	// Current Jump State
	void Current_Jump_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected:
	// Current Charge State
	void Current_Charge_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected:
	// Current Guard State
	void Current_Guard_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected:
	// Current Hit State
	void Current_Hit_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected:
	// Judge Target Gaurd State
	_bool Judge_Target_GuardState(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Judge Succes Throw
	_bool	Judge_Succes_Throw(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Judge Air Attack
	_bool	Judge_Air_Attack(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Judge Air Attack Combo
	_bool Judge_Air_AttacKCombo(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Judge Target Damaged State
	_bool Judge_TargetDamgedState(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
	// Target Dash
	_bool Judge_TargetDashState(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected:
	// Atk Combo
	void Attack_Combo(const _double& _dTimeDelta, CGameInstance* _pGameInstance, class CPlayer* _pTargetPlayer, const _float& _fDistance, const _float& _fDot);
protected:
	// Just Move
	_uint Just_Move(const _uint& _iDir, const _float& _fDot, const _uint& _iForwardstate = CSabito::STATE_MOVE | CSabito::STATE_FORWARD);
	// RandomPattern 
	_uint Random_Pattern(const _uint _iValue = 1);
	// Judge Attack Combo
	_uint Judge_AttackCombo();
	// Skill Combo
	_uint Judge_SkillCombo();
	// Judge Dash
	_uint Judge_StepRLD();
	// Reay 
	void Ready_RanPat_Time();
	// Ran Check TIme
	_uint RanCheckTime();
	// Judge Charget State
	_uint Judge_Charge_State();
	// Judge Char State Dash
	_uint Judge_CharStateDash(const _string& _strType);
protected:
	CSabito*												m_pSabito = nullptr;
	// Current Animation Clip
	CAnimationClip*									m_pCurAnimationClip = nullptr;
protected:
	// Cur Next Prev State Data 
	_uint														m_iCurStateData = 0;
	_uint														m_iNextStateData = 0;
	_uint														m_iPrevStateData = 0;
	// Look Angle
	_float														m_fLookAngle = 0.f;
	// Direction
	_uint														m_iDir = 0;
	// Player In Attack Range
	_bool														m_bPlayerInAttackRange = false;
	_string													m_strInputTrigger = "";
	//  Previous State Data 
	_string													m_strPrevEnemyState = "";
	// Stack Up Time
	_double													m_dStackUpTime = 0.;
	// Check Ran TIme Vector
	_double													m_dCheck_ChangeTime_ToRanTimeVector = 0.;
	// Ran Pat Time vector
	vector< RANPATTIME>						m_RanPatTimeVector;											
	_uint														m_iIndex = 0;
	// Attack Pattern Value
	_bool														m_bAttackPatternValue = false;
	// Idle Cus Timer
	CUSTIMER												m_tIdleWaitTimer = CUSTIMER();
	CUSTIMER												m_tDashWaitTimer = CUSTIMER();
	// Step Init
	_bool														m_bInitStep = false;

	map<_string, PATTERNM_LIST>		m_PatternMap;
	_float														m_fSaveDistacne = -100.f;
protected:
	// MAX RAN TIME VECTOR 
	const _uint											MAX_RANTIME_VECTOR = 4;
	// Change Check TIme
	const _double										PLUS_CHECK_CHANGETIME = 40.;
	// Time Factor To ranTime Vector
	const _float											TIME_FACTOR_TO_RANTIME_VECTOR = 10.f;		
public:
	static CAIController_Sabito* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, 
		class CGameObject* _pGameObject);
	virtual void Free() override;
};

END