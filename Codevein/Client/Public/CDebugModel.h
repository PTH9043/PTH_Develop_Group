#pragma once
#include "CGameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END


BEGIN(Client)

class CDebugModel final : public CGameObject
{
private:
	explicit CDebugModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDebugModel(const CDebugModel& _rhs);
	virtual ~CDebugModel() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:
	// Set Model 
	void Set_Model(CModel* _pModel);
protected:
	// Readay Components
	HRESULT Ready_Components();
	// SetUp ConstantValue
	HRESULT SetUp_ConstantTable();
private:
	CShader*			m_pShaderCom			= nullptr;
	CRenderer*		m_pRendererCom	= nullptr;
	CModel*			m_pModelCom = nullptr;
public:
	static CDebugModel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END