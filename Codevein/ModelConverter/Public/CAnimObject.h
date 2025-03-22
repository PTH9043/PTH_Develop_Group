#pragma once
#include "ModelDefines.h"
#include "CGameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Model)
class CModel_FBX;

class CAnimObject final : public CGameObject
{
private:
	explicit CAnimObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CAnimObject(const  CAnimObject& _rhs);
	virtual ~CAnimObject() = default;
public:
	// Set Model
	void Set_Model_FBX(CModel_FBX* _pModelFBX);
	// Set Model
	void Set_Model(CModel* _pModel);
	// Save Animation
	void Save_Animation();
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Ready Components
	HRESULT Ready_Components();
	// Set Up Constant Table
	HRESULT SetUp_ConstantTable();
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CModel_FBX*	m_pModelFBXCom = nullptr;
	CModel*			m_pModelCom = nullptr;
public:
	static CAnimObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END