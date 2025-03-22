#pragma once
#include "CPawn.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CItem : public CPawn
{
public:
	enum ITEMTYPE
	{
		ITEM_EQUIP, ITEM_POSION, ITEM_BULLET ,ITEM_ECT, ITEM_END
	};

	typedef struct tagItemDesc
	{
		tagItemDesc() = default;
		tagItemDesc(const _wstring& _strModelName, const _wstring& _strTextureName = L"")
			: strModelName(_strModelName), strTextureName(_strTextureName)
		{}
		// Model Name 
		_wstring strModelName = L" ";
		_wstring strTextureName = L"";
	}ITEMDESC;
protected:
	explicit CItem(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CItem(const CItem& _rhs);
	virtual ~CItem() = default;
public:
	const ITEMTYPE& Get_ItemType() const { return m_eItemType; }
public:
	void Bind_Texture(CShader* _pShader, const _char* _pConstantName);
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
protected:
	CTexture*		m_pImgTexture = nullptr;
protected:
	// Item 
	ITEMTYPE		m_eItemType = ITEM_END;
	ITEMDESC		m_tItemDesc;
	_float				m_fDrawRadius = 0.f;
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
	// Extern Pawn Data 
	virtual void Extern_PawnData() override;
#pragma endregion USE_TOOL_METHOD

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END