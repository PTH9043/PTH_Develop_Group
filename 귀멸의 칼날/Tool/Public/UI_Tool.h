#pragma once
#include"Tool_Defines.h"
#include "UI_Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Tool)

class CUI_Tool final : public CUI_Base
{

protected:
	explicit CUI_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CUI_Tool(const CUI_Tool& _rhs);
	virtual ~CUI_Tool() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	_float4x4			m_ProjMatrix;
	_int				TextureIndex = 0;
	_tchar				m_szFONTS[MAX_PATH] = TEXT("");

	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;
	
};

END
