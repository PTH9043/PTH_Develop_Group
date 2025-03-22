#pragma once
#include "Item.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)
class TeOni_Te : public CItem
{
protected:
	explicit TeOni_Te(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit TeOni_Te(const CItem& _rhs);
	virtual ~TeOni_Te(void) = default;
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
	_vector Compute_MovePos_ByDist(const _double& _dTimeDelta, _vector vMoveDir, _float fDist, _float fMaxHeight = 0.f, _float fMaxHeightRatio = 0.f, _float fPlayTime = 0.f);
public:
	static TeOni_Te* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
private:
	CTransform* m_pOniTransform = nullptr;
	CNavigation* m_pNavigationCom = nullptr;
	_bool m_bDirCheck = false;
	_vector m_vDir{};
	_double m_dDeleteTime = 0.0;
	_double	m_dFallTimeAcc = 0.0;
};

END