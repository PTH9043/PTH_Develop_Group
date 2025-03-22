#pragma once
#include "CMonster.h"

BEGIN(Client)
class CGKWStateMachine;
class CGkwEpController;

class CGateKeeperWoman final : public CMonster
{
public:
	enum INIT_ANIMTYPE
	{
		INIT_IDLE, INIT_WALK, INIT_END
	};
	enum PATTERN
	{
		PAT_AVOIDBACK, PAT_AVOIDFOR, PAT_AVOIDLR,PAT_AVOIDANDATTACK, 

		PAT_AVOID_BACK_LEFT_RIGHT, PAT_AVOID_BACK_RIGHT_LEFT,

		PAT_AVOID_LEFTRIGHT, PAT_AVOID_RIGHTLEFT,

		PAT_AVOID_FOR_LEFT_RIGHT, PAT_AVOID_FOR_RIGHT_LEFT,

		PAT_AVOID_BACKBACK, PAT_AVOID_FORFOR,

		PAT_MOVELEFT, PAT_MOVERIGHT, PAT_MOVEFOR, PAT_MOVEBACK,
		
		PAT_MOVE_BACKLEFT, PAT_MOVE_BACKRIGHT, PAT_MOVE_FORLEFT, PAT_MOVE_FORRIGHT,

		PAT_END
	};
	enum ATK_INDEX
	{
		ATK_ATTAKAVOID, ATK_ATTACKJUMP, ATK_ATTACKNORMAL_1, ATK_ATTACKNORMAL_2,
		
		ATK_ATTACKNORMAL_3, ATK_ATTACKROTATION,
		
		ATK_ATKSLASH_1, ATK_ATKSLASH_2, ATK_ATKSLASH_3, ATK_ATTACKSTORNG,
		
		ATK_ATTACKTHRUST, ATK_ATTACKSHOOT, ATK_BARRIER, ATK_ATTACKEXPLOSION, ATK_ATTACKSTORM, ATK_ATTACKRANGESHOOT,
		
		ATK_ATKTHRUSH_AFTER_ATKKNORMAL2, ATK_ATKTHRUSH_AFTER_ATKKNORMAL3,
		
		ATK_ATKTHRUSH_AFTER_ATKKSLASH2, ATK_ATKTHRUSH_AFTER_ATKKSLASH3,

		ATK_STRONG_AFTER_ATKNORMAL2, ATK_STRONG_AFTER_ATKNORMAL3,
		ATK_STRONG_AFTER_ATKSLASH2, ATK_STRONG_AFTER_ATKSLASH3,

		ATK_FAVOID_ATKNORMAL2, ATK_FAVOID_ATKNORMAL3,

		ATK_FAVOID_ATKSLASH2, ATK_FAVOID_ATKSLASH3,

		ATK_ATKNORMAL2_AFTER_ROTATIONATK, ATK_ATKSLASH2_AFTER_ROTATIONATK,

		ATK_BACK_ATTACKJUMP, 

		ATK_AVOIDFOR_ATTACKJUMP, 

		ATK_AVOIDL_ATKNORMAL2, ATK_AVOIDR_ATKNORMAL2,

		ATK_AVOIDL_ATKNORMAL2_STRONG, ATK_AVOIDR_ATKNORMAL2_STRONG,
		ATK_AVOIDL_ATKSLASH2_STRONG, ATK_AVOIDR_ATKSLASH2_STRONG,

		ATK_AVOIDL_ATKNORMAL2_ROTATE, ATK_AVOIDR_ATKNORMAL2_ROTATE,
		ATK_AVOIDL_ATKSLASH2_ROTATE, ATK_AVOIDR_ATKSLASH2_ROTATE,

		ATK_END
	};
	typedef struct tagGateKeeperWoman
	{
		CGameObject_Client::GOBCLIENTDESC	tGobDesc;
		PAWNSTATUS													tPawnStatus;
		CMonster::MONSTERDESC							tMonsterDesc;
		INIT_ANIMTYPE												eInitAnimType = INIT_END;
	}GATEKEEPERWOMANDESC;
private:
	explicit CGateKeeperWoman(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CGateKeeperWoman(const CGateKeeperWoman& _rhs);
	virtual ~CGateKeeperWoman() = default;
public:
	virtual const char* Get_Name() { return "GateKeeperWoman"; }
	virtual const _tchar* Get_Proto() { return PROTO_GOB_GATEKEEPERWOMAN; }
	virtual const _tchar* Get_Layer() { return LAYER_MONSTER; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	// Damaged
	virtual HRESULT Damaged(CPawn* _pDamageOwner) override;
protected:
	// Ready Component
	virtual HRESULT Ready_Component() override;
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable() override;
	// Ready State Machine
	virtual HRESULT Ready_Others() override;
protected:
	// Caulculate Offset Position
	void Calculate_OffsetPosition();
	// Calculate Bind Retarget
	void Bind_Retarget();
	// Bind Dodge Pos
	void Bind_Dodge_Position();

	virtual void Bind_Supplement_Position() override;
protected:
	virtual void Add_AroundPossibleAttackPlayer() override;

	virtual void Restrict_AnimationToMoveOffset(CMonsterState* _pState) override;
private:
	// IDLE State
	void IDLE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void FIND_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstnace);
	void RETARGET_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void DODGE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void MOVE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void RUN_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void ATTACK_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void DAMAGED_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void DIE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
private:
	void Attack_Move(const _double& _dTimeDelta);
	void Avoid_Move(const _double& _dTImeDelta);
private:
	CGKWStateMachine*					m_pGKWStateMachine = nullptr;
	CGkwEpController*						m_pGkwEpController = nullptr;
	_bool													m_bChangeDodgeMatrix = false;
	_bool													m_bEndDodge = false;
	_bool													m_bBindRetarget = false;
	// Move Timer
	CUSTIMER											m_tMoveTimer;
	// Judge Turn
	_ubyte												m_byJudgeTurn = 0;
	// OverTime
	_float													m_fOverTime_AttackMove = 15.f;
public:
#pragma region USE_TOOL_METHOD
	// Extern EditValue
	virtual _bool Extern_EditValue();
	// Extern Hierakey Value
	virtual _bool Extern_HierakeyView(const string& _str);
	// Create Self 
	virtual HRESULT Create_Self(CGameObject** _ppOut);
	// Delete Self
	virtual HRESULT Delete_Self();
	// Save Data 
	virtual HRESULT Save_Data();
	// Load Data 
	virtual HRESULT Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData = nullptr);
	// Get Picking
	virtual _bool Is_Picking(_float3* _pOut = nullptr);
	// Reset Data 
	virtual void Reset_Data();
protected:
	void Extern_InitAnation();
	// Control ColliderData
	virtual void Control_ColliderData() override;
#pragma endregion USE_TOOL_METHOD

#pragma region USE_TOOL_VARIABLE
	_int				m_iModelID = 0;
	_int				m_iAnimID = 0;
#pragma endregion USE_TOOL_VARIABLE
public:
	static CGateKeeperWoman* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;
};

END