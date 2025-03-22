#pragma once
#include "CWeapon.h"

BEGIN(Client)

class COneHandSword : public CWeapon
{
public:
	typedef struct tagOneHandSwordDesc
	{
		ITEMDESC				tItemDesc;
		EQUIPDESC			tEquipDesc;
		WEAPONDESC		tWeaponDesc;
	}ONEHANDSWRODDESC;
protected:
	explicit COneHandSword(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit COneHandSword(const COneHandSword& _rhs);
	virtual ~COneHandSword() = default;
public:
	virtual const _char* Get_Name() override { return "OneHandSword"; }
	virtual const _tchar* Get_Layer() override { return LAYER_WEAPON; }
	virtual const _tchar* Get_Proto() override { return PROTO_GOB_ONEHANDSWORD; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Ready Component
	virtual HRESULT Ready_Component() override;
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable() override;
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
#pragma endregion USE_TOOL_METHOD
protected:
	// TwoHandSwordDesc
	ONEHANDSWRODDESC m_tOneHandSwordDesc;
public:
	// Weapon
	static COneHandSword* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END