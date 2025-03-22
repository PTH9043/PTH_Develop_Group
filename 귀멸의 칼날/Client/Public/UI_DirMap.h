
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

class CUI_DirMap final : public CUI_Base
{

protected:CUI_DirMap(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
		 explicit CUI_DirMap(const CUI_DirMap& _rhs);
		 virtual ~CUI_DirMap() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	virtual void Extern_ImGuiClass() override;

public:
	void Dir_NPC();
	void Dir_Oni();
	void Dir_Destination();

public:
	void Set_IconRender(_bool render) { IconRender = render; }

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	
private:
	_float4x4			m_ProjMatrix;
	_float theta = 0.f;
	_float Burgertheta = 0.f;
	_float3 fCameraPos = { 0.f,0.f,0.f };
	_bool  IconRender = false;
	_float3 fOniPos = { 0.f,0.f,0.f };
	_tchar	m_szFONTS[MAX_PATH] = TEXT("");
	_tchar	m_szNAMES[MAX_PATH] = TEXT("");
	_float3 fNPCLengrh = { 0.f,0.f,0.f };
	_float3 fBurgerLengrh = { 0.f,0.f,0.f };
	_float3 fOniLengrh = { 0.f,0.f,0.f };
	

	static _uint m_iCurOniLayer;
	

	vector <_float3> OniLengrhlist;//거리 리스트 
		
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_DirMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END
