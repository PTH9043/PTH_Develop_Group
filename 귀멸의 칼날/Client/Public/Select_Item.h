#pragma once
#include"Client_Defines.h"
#include "Item.h"


BEGIN(Client)

class CSelect_Item : public CItem
{

protected:
	 CSelect_Item(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	 CSelect_Item(const CSelect_Item& _rhs);
	virtual ~CSelect_Item() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

public:
	void Set_Render(_bool render) { RenderModel = render; }
	
private: 
	_bool RenderModel = false;
	_float RenderItem = 0.f;
	
protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;


public:
	static  CSelect_Item* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END