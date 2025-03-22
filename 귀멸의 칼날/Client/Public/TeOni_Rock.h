#pragma once
#include "Item.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)
class CTeOni_Rock : public CItem
{
protected:
	explicit CTeOni_Rock(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CTeOni_Rock(const CItem& _rhs);
	virtual ~CTeOni_Rock(void) = default;
public:
	virtual HRESULT NativeConstruct_Prototype(void) override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(void) override;
	virtual HRESULT	RenderLight(void) override;
protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable(void) override;
	// SetUp Components
	virtual HRESULT SetUp_Components(void) override;
	//  Set Up Load Data 
	virtual HRESULT SetUp_LoadData(void* _pArg);
	virtual HRESULT Delete_Self(void);
public:
	static CTeOni_Rock* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
private:
	CGameObject* m_pPlayer = nullptr;
	CTransform* m_pOniTransform = nullptr;
	CNavigation* m_pNavigationCom = nullptr;
	_bool m_bDirCheck = false; 
	_bool m_bCreateCheck = false; 
	_vector m_vDir{};
	_double m_dDeleteTime = 0.0;
};

END