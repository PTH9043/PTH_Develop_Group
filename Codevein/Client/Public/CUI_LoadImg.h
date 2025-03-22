#pragma once
#include "Client_Defines.h"
#include "CUserInterface.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_LoadImg final : public CUserInterface
{
protected:
	explicit CUI_LoadImg(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CUI_LoadImg(const CUI_LoadImg& _rhs);
	virtual ~CUI_LoadImg() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Ready Component
	HRESULT Ready_Component();
	// Setting Constant Table
	HRESULT SetUp_ConstantTable(CRectTransform* _pRectTransform, CTransform* _pTransform);
protected:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRenderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIRectCom = nullptr;
protected:
	_float m_fLoadMaxTime = 0.8f;
	_float m_fLoadPlusTime = 0.f;
public:
	static CUI_LoadImg* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END