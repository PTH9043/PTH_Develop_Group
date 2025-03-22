#pragma once
#include "CMonster.h"
#include "CWolfDevilStateMachine.h"

BEGIN(Client)

class CWolfDevil final : public CMonster
{
public:
	enum INIT_ANIMTYPE
	{
		INIT_IDLE, INIT_SIT, INIT_EAT, INIT_WALK, INIT_END
	};

	enum PATTERN
	{
		PAT_1, PAT_2, PAT_3, PAT_4, PAT_END
	};

	enum ATK_INDEX
	{
		ATK_JUMP, ATK_NORMAL, ATK_SP, ATK_END
	};

	typedef struct tagWolfDevilDesc
	{
		CGameObject_Client::GOBCLIENTDESC	tGobDesc;
		PAWNSTATUS													tPawnStatus;
		CMonster::MONSTERDESC							tMonsterDesc;
		INIT_ANIMTYPE												eInitAnimType = INIT_END;
	}WOLFDEVILDESC;
private:
	explicit CWolfDevil(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CWolfDevil(const CWolfDevil& _rhs);
	virtual ~CWolfDevil() = default;
public:
	virtual const char* Get_Name() { return "WolfDevil"; }
	virtual const _tchar* Get_Proto() { return PROTO_GOB_WOLFDEVIL; }
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
private:
	// Ready Component
	virtual HRESULT Ready_Component() override;
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable() override;
	// Ready State Machine
	virtual HRESULT Ready_Others() override;
protected:
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
	void RUN_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void ATTACK_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void DAMAGED_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void DIE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
private:
	CWolfDevilStateMachine*			m_pWolfDevilStateMachine = nullptr;
	INIT_ANIMTYPE								m_eInitIdleType = INIT_END;
	_bool													m_bChangeDodgeMatrix = false;
	_bool													m_bEndDodge = false;
	_bool													m_bChangeRetargetTurnMatrix = false;
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
	void Extern_ModelType();
#pragma endregion USE_TOOL_METHOD

#pragma region USE_TOOL_VARIABLE
	_int				m_iModelID = 0;
	_int				m_iAnimID = 0;
#pragma endregion USE_TOOL_VARIABLE
public:
	static CWolfDevil* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;
};

END