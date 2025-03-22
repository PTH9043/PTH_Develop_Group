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

class CMinimap final : public CUserInterface
{
protected:
	explicit CMinimap(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CMinimap(const CMinimap& _rhs);
	virtual ~CMinimap() = default;

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
	HRESULT SetUp_ConstantTable();
protected:
	class CCamera_Minimap*		m_pCamMinimap = nullptr; 
protected:
	CShader*									m_pShaderCom = nullptr;
	CRenderer*								m_pRenderCom = nullptr;
	CTexture*									m_pTextureCom = nullptr;
	CVIBuffer_Rect*						m_pVIRectCom = nullptr;
protected:
	const _tchar* RENDERTARGET		= L"MinmapRenderTarget";
	const _tchar* MRT								= L"UI";
public:
	static CMinimap* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END