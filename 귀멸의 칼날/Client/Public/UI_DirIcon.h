#pragma once
#include"Client_Defines.h"
#include "UI_Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_DirIcon final : public CUI_Base
{

protected:CUI_DirIcon(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
		 explicit CUI_DirIcon(const CUI_DirIcon& _rhs);
		 virtual ~CUI_DirIcon() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

public:
	void Dir_Oni();

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	_float4x4			m_ProjMatrix;

	_tchar	m_szFONTS[MAX_PATH] = TEXT("");
	_uint m_iCurOniLayer = 0;
	vector <_float3> OniLengrhlist;//거리 리스트 
	

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_DirIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END
