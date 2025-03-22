#pragma once
#include "Item.h"

BEGIN(Client)

class CSheath final : public CItem
{
protected:
	explicit CSheath(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CSheath(const CSheath& _rhs);
	virtual ~CSheath() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	virtual HRESULT	RenderLight() override;

protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;

public:
	static  CSheath* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END