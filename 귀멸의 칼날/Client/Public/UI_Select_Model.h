#pragma once
#include"Client_Defines.h"
#include "UI_Base.h"
#include "Select_Item.h"

BEGIN(Engine)
class CAnimator;
class CModel;
class CRenderer;
class CShader;
END

BEGIN(Client)

class CUI_Select_Model final : public CUI_Base
{
public:
	enum ModelTag {MODEL_TANJIRO, MODEL_ZENITSU, MODEL_SABITO, MODEL_SHINOBU, MODEL_AKAZA, MODEL_END};
	enum ModelState { SELECT, STATE_END };
protected:
	explicit CUI_Select_Model(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CUI_Select_Model(const CUI_Select_Model& _rhs);
	virtual ~CUI_Select_Model() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Change_Model(_uint State);
	
protected:
	// Set Up Constant
	 HRESULT SetUp_ConstantTable() ;
	// SetUp Components
	 HRESULT SetUp_Components() ;

private:
	CShader* m_pShader = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CModel* m_pModel = nullptr;
	CModel* m_pAllAnimModel = nullptr;
	CAnimator* m_pAnimator = nullptr;
	CSelect_Item* m_pSheath = nullptr;
	CSelect_Item* m_pSword = nullptr;
	_float4x4 m_PrevWorldMatrix;

	_uint PrevState = STATE_END;
	_uint CurrentState = STATE_END;

public:
	void Set_Render(_bool render) { RenderModel = render; }
	void Setting_Trigger();

private:
	_bool RenderModel = false;
	_int Player = 0;

	_bool IsSelect = false;

public:
	vector <CModel*> m_ModelList;
	vector <CAnimator*> m_AnimatorList;
	vector <CSelect_Item*> m_SwordList;
	vector <CSelect_Item*> m_SheathList;

public:
	static CUI_Select_Model* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END