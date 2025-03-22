#pragma once
#include "CMonster.h"
#include "CGiantDevilStateMachine.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CPlayer;

class CGiantDevil : public CMonster
{
public:
	enum EQUIP_WEAPONTYPE
	{
		EW_HAMMER, EW_SWORD, EW_END
	};

	enum INIT_ANIMTYPE
	{
		INIT_IDLE, INIT_CROUCH, INIT_SIT, INIT_END
	};

	enum PATTERN
	{
		PAT_N1, PAT_N2, PAT_S1, PAT_S2, PAT_SS1, PAT_SS2, PAT_END
	};

	typedef struct tagGiantDevilDesc
	{
		CGameObject_Client::GOBCLIENTDESC	tGobDesc;
		CPawn::PAWNSTATUS									tPawnDesc;
		CMonster::MONSTERDESC							tMonsterDesc;
		EQUIP_WEAPONTYPE									eEquipWeaponType = EW_SWORD;
		INIT_ANIMTYPE												eInitAnimType = INIT_END;
	}GIANTDEVILDESC;
private:
	explicit CGiantDevil(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CGiantDevil(const CGiantDevil& _rhs);
	virtual ~CGiantDevil() = default;
public:
	virtual const char* Get_Name() { return "GiantDevil"; }
	virtual const _tchar* Get_Proto() { return PROTO_GOB_GIANTDEVIL; }
	virtual const _tchar* Get_Layer() { return LAYER_MONSTER; }
public:
	const EQUIP_WEAPONTYPE& Get_EquipWeaponType() const { return m_eWeaponType; }
	const INIT_ANIMTYPE& Get_InitAnimType() const { return m_eInitIdleType; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
private:
	// Ready Component
	virtual HRESULT Ready_Component() override;
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable() override;
	// Ready State Machine
	virtual HRESULT Ready_Others();
protected:
	void Calculate_OffsetPosition();
	void Bind_Attack_Postion();
	void Bind_Supplement_Position();

private:
	//  Giant State mahcine
	CGiantDevilStateMachine* m_pGiantDevilStateMachine = nullptr;
	// Equip Weapon Type
	EQUIP_WEAPONTYPE m_eWeaponType = EW_SWORD;
	// Init Idle Type
	INIT_ANIMTYPE			  m_eInitIdleType = INIT_END;
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
	// Judge WeaponType
	void Extern_JudgeMonsterAndWeaponType();
	void Extern_InitAnation();
#pragma endregion USE_TOOL_METHOD

#pragma region USE_TOOL_VARIABLE
	_int				m_iModelID = 0;
	_int				m_iAnimID = 0;
#pragma endregion USE_TOOL_VARIABLE
public:
	static CGiantDevil* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;
};

END