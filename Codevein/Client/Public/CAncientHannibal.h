#pragma once
#include "CMonster.h"
#include "CAncHanibalStateMachine.h"

BEGIN(Engine)
class CHierarchyNode;
class CCollider;
END

BEGIN(Client)
class CParticle;
class CAncEPController;
class CCinematicCamera;
class CPotal;

class CAncientHannibal final : public CMonster
{
public:
	enum INIT_ANIMTYPE
	{
		INIT_IDLE, INIT_WALK, INIT_END
	};
	enum PATTERN
	{
		PAT_MOVEFORWARD, PAT_MOVERIGHT, PAT_MOVELEFT, PAT_MOVEBACK, 
		
		PAT_MOVEAVOIDFRONT1, PAT_BACKAVOIDMOVE1, PAT_JUSTMOVEBACK,
		
		PAT_RUNFORWARD, PAT_RUNRIGHT, PAT_RUNLEFT,  
		PAT_BACKDOGDE,  PAT_TURNSTATE1, PAT_TURNSTATE2, 
		
		PAT_NORMALDOGDE, PAT_BACKANDSHOOT, PAT_DOGDEANDWEAPONCOMBO, PAT_DOGDEANDSNAKE,  PAT_DOGDEANDJUMPATTACK,
		PAT_DOUBLEDOGDEAFTERSNAKE, PAT_DOGDESHOOT,

		PAT_END
	};
	enum ATK_INDEX
	{
		ATK_FIRSTORM, ATK_GILD_N, ATK_GILD_SP, ATK_HALFTURN,
		ATK_HOOK, ATK_JUMP, ATK_POINTSHOOT, ATK_SLASH, ATK_STEP,  ATK_STAMP, 
		ATK_TAIL, ATK_WEAPONCOMBO1, ATK_SNAKE,  ATK_ONEHAND,
		ATK_END
	};
	typedef struct tagAncientHannibal
	{
		CGameObject_Client::GOBCLIENTDESC	tGobDesc;
		PAWNSTATUS													tPawnStatus;
		CMonster::MONSTERDESC							tMonsterDesc;
		INIT_ANIMTYPE												eInitAnimType = INIT_END;
	}ANCHANNIBALDESC;
private:
	explicit CAncientHannibal(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CAncientHannibal(const CAncientHannibal& _rhs);
	virtual ~CAncientHannibal() = default;
public:
	virtual const char* Get_Name() { return "AncientHannibal"; }
	virtual const _tchar* Get_Proto() { return PROTO_GOB_ANCIENTHANNIBAL; }
	virtual const _tchar* Get_Layer() { return LAYER_MONSTER; }
public:
	const INIT_ANIMTYPE& Get_InitAnimType() const { return m_eInitIdleType; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	// Damaged
	virtual HRESULT Damaged(CPawn* _pDamageOwner) override;
	virtual _bool Is_Collision(CCollider* _pCollider) override;
protected:
	// Ready Component
	virtual HRESULT Ready_Component() override;
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable() override;
	// Ready State Machine
	virtual HRESULT Ready_Others() override;
	// Find Offset
	virtual HRESULT Find_OffsetNode() override;
protected:
	// Caulculate Offset Position
	void Calculate_OffsetPosition();
	// Bind Dodge Pos
	void Bind_Dodge_Position();
protected:
	virtual void Add_AroundPossibleAttackPlayer() override;
	// Attack Strong Count
	void Judge_AttackStrongCount();
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
protected:
	void Next_Retarget(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void Next_Dogde(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void NextMove(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void NextRun(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
private:
	CAncHanibalStateMachine*		m_pAncHannibalStateMachine = nullptr;
	CAncEPController*							m_pAncEpController = nullptr;
	CPotal*												m_pPotal = nullptr;
	INIT_ANIMTYPE								m_eInitIdleType = INIT_END;
	_bool													m_bChangeDodgeMatrix = false;
	_bool													m_bEndDodge = false;
	_bool													m_bChangeRetargetTurnMatrix = false;
	_uint													m_iPrevAttackIndex = 0;
	// Move Timer
	CUSTIMER											m_tMoveTimer;
	CUSTIMER											m_tSnakeTimer;
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
	static CAncientHannibal* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;
};

END