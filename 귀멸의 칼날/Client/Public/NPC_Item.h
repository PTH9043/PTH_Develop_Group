#pragma once
#include"Client_Defines.h"
#include "Item.h"



BEGIN(Client)

class CNPC_Item : public CItem
{
public:
	enum Type {Item1, Item2, Item_End};
protected:
	CNPC_Item(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	CNPC_Item(const CNPC_Item& _rhs);
	virtual ~CNPC_Item() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	virtual void Extern_ImGuiClass() override;

protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;

public:
	void Setting_Pos(_int Type);
	
private:
	_uint m_iType = Item_End;
	

public:
	static  CNPC_Item* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END