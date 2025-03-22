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

class CKnownKey_UI : public CUserInterface
{
protected:
	explicit CKnownKey_UI(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CKnownKey_UI(const CKnownKey_UI& _rhs);
	virtual ~CKnownKey_UI() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Ready Component
	HRESULT Ready_Component();
	// Set Up Component
	HRESULT SetUp_Component();
protected:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRenderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIRectCom = nullptr;

public:
	static CKnownKey_UI* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END