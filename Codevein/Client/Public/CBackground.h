#pragma once
#include "Client_Defines.h"
#include "CUserInterface.h"

BEGIN(Engine)
class CRectTransform;
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackground : public CUserInterface
{
protected:
	explicit CBackground(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CBackground(const CBackground& _rhs);
	virtual ~CBackground() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
private:
	// Setting Constant Table
	HRESULT SetUp_ConstantTable();
	// Ready Component
	HRESULT Ready_Component();
private:
	CShader*			 	 m_pShaderCom = nullptr;
	CRenderer*			 m_pRenderCom = nullptr;
	CTexture*				 m_pTextureCom = nullptr; 
	CVIBuffer_Rect*  m_pVIRectCom = nullptr;
public:
	static CBackground* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg)  override; 
	virtual void Free() override;
};

END