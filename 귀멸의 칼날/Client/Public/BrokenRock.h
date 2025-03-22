#pragma once
#include "Item.h"

BEGIN(Client)
class CBrokenRock : public CItem
{
protected:
	explicit CBrokenRock(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CBrokenRock(const CItem& _rhs);
	virtual ~CBrokenRock(void) = default;
public:
	virtual HRESULT NativeConstruct_Prototype(void) override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(void) override;
	virtual HRESULT	RenderLight(void) override;
	virtual void Extern_ImGuiClass() override;
protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable(void) override;
	// SetUp Components
	virtual HRESULT SetUp_Components(void) override;
	//  Set Up Load Data 
	virtual HRESULT SetUp_LoadData(void* _pArg);
	virtual HRESULT Delete_Self(void);
public:
	static CBrokenRock* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
private:
	_bool m_bRenderCheck = false;
};

END