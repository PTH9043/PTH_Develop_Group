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

class CMiniGame_Model final : public CUI_Base
{
public:
	enum ModelTag { NURSE_0, NURSE_1, NURSE_2, MODEL_END };

protected:
	explicit CMiniGame_Model(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CMiniGame_Model(const CMiniGame_Model& _rhs);
	virtual ~CMiniGame_Model() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Extern_ImGuiClass() override;

public:
	void Change_Model(_uint State);
	void Setting_Trigger(const _double& _dTimeDelta);

protected:
	// Set Up Constant
	HRESULT SetUp_ConstantTable();
	// SetUp Components
	HRESULT SetUp_Components();

private:
	CShader* m_pShader = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CModel* m_pModel = nullptr;
	CModel* m_pAllAnimModel = nullptr;
	CAnimator* m_pAnimator = nullptr;


public:
	vector <CModel*> m_ModelList;
	vector <CAnimator*> m_AnimatorList;

public:
	_int Get_type() { return type; }

private:
	_int type = MODEL_END;
	_double TriggerTime = 0.0;
	_double Trigger = 63.0;

public:
	static CMiniGame_Model* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
