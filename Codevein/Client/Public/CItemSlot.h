#pragma once
#include "CSlot.h"

BEGIN(Client)
class CItem;


class CItemSlot final : public CSlot
{
protected:
	explicit CItemSlot(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CItemSlot(const CItemSlot& _rhs);
	virtual ~CItemSlot() = default;

public:
	void Set_Item(CItem* _pItem) { this->m_pItem = _pItem; }

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Ready Component
	virtual HRESULT Ready_Component() override;
	// Setting Constant Table
	virtual HRESULT SetUp_ConstantTable() override;
protected:
	// Item
	CItem* m_pItem = nullptr;
public:
	static CItemSlot* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END